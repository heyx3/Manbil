#include "WorldObject.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Rendering/Data Nodes/DataNodes.hpp"


const Vector3f WorldObject::LightDir = Vector3f(-2.0f, 1.0f, -5.0f).Normalized();
const float WorldObject::AmbientLight = 0.5f,
            WorldObject::DiffuseLight = 1.0f - AmbientLight;


Vector3f RemapMayaVert(const aiVector3D& in)
{
    return Vector3f(in.x, in.z, in.y);
}


WorldObject::WorldObject(GeoSet geoInfo, std::string& outError)
    : Mat(0), getUVs(!geoInfo.UseWorldPosUV), useNormalMaps(geoInfo.UseNormalMap),
      DiffTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PS_8U, true),
      NormalTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PS_8U, true)
{
    MyMesh.SubMeshes.push_back(MeshData(false, PT_TRIANGLE_LIST));
    outError = LoadMesh(geoInfo.MeshFile, !geoInfo.UseWorldPosUV,
                        geoInfo.UseNormalMap, MyMesh.SubMeshes[0]);
    if (!outError.empty())
    {
        return;
    }

    ShaderGenerator::GeneratedMaterial genM = LoadMaterial(geoInfo);
    if (!genM.ErrorMessage.empty())
    {
        outError = genM.ErrorMessage;
        return;
    }
    Mat = genM.Mat;

    DiffTex.Create();
    if (!DiffTex.SetDataFromFile("Content/Old Ones/Diffuse Maps/" + geoInfo.DiffuseTexFile,
                                 outError))
    {
        return;
    }
    //Add anisotropic filtering, which isn't built-in to the texture system yet.
    if (glewIsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
    {
        DiffTex.Bind();
        GLfloat largest;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largest);
    }
    Params.Texture2Ds["u_diffuseTex"].Texture = DiffTex.GetTextureHandle();
    if (geoInfo.UseNormalMap)
    {
        NormalTex.Create();
        if (!NormalTex.SetDataFromFile("Content/Old Ones/Normal Maps/" + geoInfo.NormalTexFile,
                                       outError))
        {
            return;
        }
        Params.Texture2Ds["u_normalTex"].Texture = NormalTex.GetTextureHandle();
    }
}
WorldObject::~WorldObject(void)
{
    if (Mat != 0)
    {
        delete Mat;
        Mat = 0;
    }
}


RenderIOAttributes WorldObject::GetVertexInputs(bool getUVs, bool hasNormalMaps)
{
    if (getUVs)
    {
        if (hasNormalMaps)
        {
            return RenderIOAttributes(RenderIOAttributes::Attribute(3, false, "vIn_Pos"),
                                      RenderIOAttributes::Attribute(3, true, "vIn_Normal"),
                                      RenderIOAttributes::Attribute(3, true, "vIn_Tangent"),
                                      RenderIOAttributes::Attribute(3, true, "vIn_Bitangent"),
                                      RenderIOAttributes::Attribute(2, false, "vIn_UV"));
        }
        else
        {
            return RenderIOAttributes(RenderIOAttributes::Attribute(3, false, "vIn_Pos"),
                                      RenderIOAttributes::Attribute(3, true, "vIn_Normal"),
                                      RenderIOAttributes::Attribute(2, false, "vIn_UV"));
        }
    }
    else
    {
        if (hasNormalMaps)
        {
            return RenderIOAttributes(RenderIOAttributes::Attribute(3, false, "vIn_Pos"),
                                      RenderIOAttributes::Attribute(3, true, "vIn_Normal"),
                                      RenderIOAttributes::Attribute(3, true, "vIn_Tangent"),
                                      RenderIOAttributes::Attribute(3, true, "vIn_Bitangent"));
        }
        else
        {
            return RenderIOAttributes(RenderIOAttributes::Attribute(3, false, "vIn_Pos"),
                                      RenderIOAttributes::Attribute(3, true, "vIn_Normal"));
        }
    }
}
void WorldObject::Render(RenderInfo& info)
{
    Mat->Render(info, &MyMesh, Params);
}

std::string WorldObject::LoadMesh(const std::string& meshFile, bool getUVs, bool hasNormalMaps,
                                  MeshData& outDat)
{
    //Load the mesh data.
    Assimp::Importer importer;
    unsigned int flags = aiProcessPreset_TargetRealtime_Fast;
    const aiScene* scene = importer.ReadFile("Content/Old Ones/Meshes/" + meshFile, flags);

    std::string err = importer.GetErrorString();
    if (!err.empty())
    {
        return "Error reading mesh file '" + meshFile + "': " + err;
    }
    //Make sure the mesh was loaded properly.
    if (scene == 0)
    {
        return "Error importing mesh '" + meshFile + "': " + importer.GetErrorString();
    }
    if (scene->mNumMeshes != 1)
    {
        std::string nMeshes = std::to_string(scene->mNumMeshes);
        importer.FreeScene();
        return "There are " + nMeshes + " meshes in the file '" + meshFile + "'!";
    }

    //Make sure the mesh is defined properly.
    aiMesh* mesh = scene->mMeshes[0];
    if (!mesh->HasPositions())
    {
        importer.FreeScene();
        return "Mesh '" + meshFile + "' doesn't have a 'position' attribute!";
    }
    if (getUVs && !mesh->HasTextureCoords(0))
    {
        importer.FreeScene();
        return "Mesh '" + meshFile + "' doesn't have a 'UV' attribute!";
    }
    if (!mesh->HasNormals())
    {
        importer.FreeScene();
        return "Mesh '" + meshFile + "' doesn't have a 'normal' attribute!";
    }
    if (!mesh->HasFaces())
    {
        importer.FreeScene();
        return "Mesh '" + meshFile + "' doesn't have triangles!";
    }
    if (hasNormalMaps)
    {
        if (!mesh->HasTangentsAndBitangents())
        {
            importer.FreeScene();
            return "Mesh '" + meshFile + "' doesn't have tangents/bitangents!";
        }
    }

    //Put the vertices into the vertex buffer.
    RenderIOAttributes attrs = GetVertexInputs(getUVs, hasNormalMaps);
    if (getUVs)
    {
        if (hasNormalMaps)
        {
            struct VertClass
            {
                Vector3f pos, normal, tangent, bitangent;
                Vector2f uv;
            };
            std::vector<VertClass> verts;
            verts.resize(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                verts[i].pos = RemapMayaVert(mesh->mVertices[i]);
                verts[i].normal = RemapMayaVert(mesh->mNormals[i]);
                verts[i].tangent = RemapMayaVert(mesh->mTangents[i]);
                verts[i].bitangent = verts[i].normal.Cross(verts[i].tangent);
                verts[i].uv = *(Vector2f*)(&mesh->mTextureCoords[0][i].x);
            }
            outDat.SetVertexData(verts, MeshData::BUF_STATIC, attrs);
        }
        else
        {
            struct VertClass
            {
                Vector3f pos, normal;
                Vector2f uv;
            };
            std::vector<VertClass> verts;
            verts.resize(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                verts[i].pos = RemapMayaVert(mesh->mVertices[i]);
                verts[i].normal = RemapMayaVert(mesh->mNormals[i]);
                verts[i].uv = *(Vector2f*)(&mesh->mTextureCoords[0][i].x);
            }
            outDat.SetVertexData(verts, MeshData::BUF_STATIC, attrs);
        }
    }
    else
    {
        if (hasNormalMaps)
        {
            struct VertClass
            {
                Vector3f pos, normal, tangent, bitangent;
            };
            std::vector<VertClass> verts;
            verts.resize(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                verts[i].pos = RemapMayaVert(mesh->mVertices[i]);
                verts[i].normal = RemapMayaVert(mesh->mNormals[i]);
                verts[i].tangent = RemapMayaVert(mesh->mTangents[i]);
                verts[i].bitangent = verts[i].normal.Cross(verts[i].tangent);
            }
            outDat.SetVertexData(verts, MeshData::BUF_STATIC, attrs);
        }
        else
        {
            struct VertClass
            {
                Vector3f pos, normal;
            };
            std::vector<VertClass> verts;
            verts.resize(mesh->mNumVertices);
            for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
            {
                verts[i].pos = RemapMayaVert(mesh->mVertices[i]);
                verts[i].normal = RemapMayaVert(mesh->mNormals[i]);
            }
            outDat.SetVertexData(verts, MeshData::BUF_STATIC, attrs);
        }
    }

    //Put the indices into the index buffer.
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace& fce = mesh->mFaces[i];
        if (fce.mNumIndices != 3)
        {
            std::string nIndices = std::to_string(fce.mNumIndices);
            importer.FreeScene();

            return "Mesh '" + meshFile + "'s face #" + std::to_string(i) +
                   " has " + nIndices + " vertices";
        }
        
        indices.push_back(fce.mIndices[0]);
        indices.push_back(fce.mIndices[1]);
        indices.push_back(fce.mIndices[2]);
    }
    outDat.SetIndexData(indices, MeshData::BUF_STATIC);

    importer.FreeScene();
    return "";
}

ShaderGenerator::GeneratedMaterial WorldObject::LoadMaterial(const GeoSet& geoInfo)
{
    DataNode::ClearMaterialData();
    
    //Figure out what vertex attributes are needed.
    RenderIOAttributes attrs = GetVertexInputs(!geoInfo.UseWorldPosUV, geoInfo.UseNormalMap);
    DataLine vIn_Pos, vIn_Normal, vIn_Tangent, vIn_Bitangent, vIn_UV;
    if (geoInfo.UseWorldPosUV)
    {
        if (geoInfo.UseNormalMap)
        {
            vIn_Pos = DataLine(VertexInputNode::GetInstance(), 0);
            vIn_Normal = DataLine(VertexInputNode::GetInstance(), 1);
            vIn_Tangent = DataLine(VertexInputNode::GetInstance(), 2);
            vIn_Bitangent = DataLine(VertexInputNode::GetInstance(), 3);
        }
        else
        {
            vIn_Pos = DataLine(VertexInputNode::GetInstance(), 0);
            vIn_Normal = DataLine(VertexInputNode::GetInstance(), 1);
        }
    }
    else
    {
        if (geoInfo.UseNormalMap)
        {
            vIn_Pos = DataLine(VertexInputNode::GetInstance(), 0);
            vIn_Normal = DataLine(VertexInputNode::GetInstance(), 1);
            vIn_Tangent = DataLine(VertexInputNode::GetInstance(), 2);
            vIn_Bitangent = DataLine(VertexInputNode::GetInstance(), 3);
            vIn_UV = DataLine(VertexInputNode::GetInstance(), 4);
        }
        else
        {
            vIn_Pos = DataLine(VertexInputNode::GetInstance(), 0);
            vIn_Normal = DataLine(VertexInputNode::GetInstance(), 1);
            vIn_UV = DataLine(VertexInputNode::GetInstance(), 2);
        }
    }

    DataNode::VertexIns = attrs;

    //Figure out what values need to be output to the fragment shader.
    DataNode::Ptr screenPos = SpaceConverterNode::ObjPosToScreenPos(vIn_Pos, "objPosToScreen"),
                  worldPos = SpaceConverterNode::ObjPosToWorldPos(vIn_Pos, "objPosToWorld");
    DataNode::Ptr worldPosHorz(new SwizzleNode(worldPos, SwizzleNode::C_X, SwizzleNode::C_Y,
                                               "worldPosHorz"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(screenPos, 1);

    DataLine fIn_Normal(FragmentInputNode::GetInstance(), 0),
             fIn_UV(FragmentInputNode::GetInstance(), 1),
             fIn_WorldPos(FragmentInputNode::GetInstance(), 2),
             fIn_Tangent(FragmentInputNode::GetInstance(), 3),
             fIn_Bitangent(FragmentInputNode::GetInstance(), 4);
    DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_Normal", vIn_Normal));
    if (geoInfo.UseWorldPosUV)
    {
        DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_UV",
                                                                    worldPosHorz));
    }
    else
    {
        DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_UV",
                                                                    vIn_UV));
    }
    DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_WorldPos", worldPos));
    if (geoInfo.UseNormalMap)
    {
        DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_Tangent",
                                                                    vIn_Tangent));
        DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_Bitangent",
                                                                    vIn_Bitangent));
    }


    //Calculate the fragment shader's output.

    DataNode::Ptr diffuseUV(new MultiplyNode(geoInfo.DiffuseTexScale, fIn_UV, "diffuseUV")),
                  normalUV(new MultiplyNode(geoInfo.NormalTexScale, fIn_UV, "normalUV"));
    DataNode::Ptr diffuseSampler(new TextureSample2DNode(diffuseUV, "u_diffuseTex", "diffuseSampler")),
                  normalSampler(new TextureSample2DNode(normalUV, "u_normalTex", "normalSampler"));
    DataNode::Ptr normalMap(new RemapNode(DataLine(normalSampler,
                                                   TextureSample2DNode::GetOutputIndex(CO_AllColorChannels)),
                                          0.0f, 1.0f, -1.0f, 1.0f, "normalMap"));

    DataNode::Ptr tempNormalCalc;
    DataLine finalNormal = fIn_Normal;
    if (geoInfo.UseNormalMap)
    {
        tempNormalCalc = DataNode::Ptr(new ApplyNormalMapNode(fIn_Normal, fIn_Tangent, fIn_Bitangent,
                                                              normalMap));
        finalNormal = DataLine(tempNormalCalc);
    }

    DataNode::Ptr brightness(new LightingNode(fIn_WorldPos, finalNormal, LightDir,
                                              "brightnessCalc", AmbientLight, DiffuseLight,
                                              geoInfo.Specular, geoInfo.SpecularIntensity));

    DataNode::Ptr finalColor(new MultiplyNode(DataLine(diffuseSampler,
                                                       TextureSample2DNode::GetOutputIndex(CO_AllChannels)),
                                              brightness,
                                              "finalColor"));

    DataNode::MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_FinalCol", finalColor));


    //Use either transparent or opaque blending.
    return ShaderGenerator::GenerateMaterial(Params,
                                             geoInfo.IsTransparent ?
                                                BlendMode::GetTransparent() :
                                                BlendMode::GetOpaque());
}