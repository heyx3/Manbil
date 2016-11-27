#include "OldOnesSkybox.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/Primitives/PrimitiveGenerator.h"
#include "WorldObject.h"


typedef OldOnesSkybox OOS;
typedef PrimitiveGenerator::CubemapVertex Vert;


OOS::OldOnesSkybox(std::string& outError)
    : tex(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, false),
      cubeMesh(false, PrimitiveTypes::PT_TRIANGLE_LIST)
{
    //Set up texture.
    std::string cubemapPath = "Content/Old Ones/Skyboxes/nwo512_",
                cubemapType = ".jpg";
    MTextureCubemap::EnableSmoothSeams();
    tex.Create();
    tex.SetDataFromFiles(cubemapPath + "left" + cubemapType,
                         cubemapPath + "front" + cubemapType,
                         cubemapPath + "bottom" + cubemapType,
                         cubemapPath + "right" + cubemapType,
                         cubemapPath + "back" + cubemapType,
                         cubemapPath + "top" + cubemapType);


    //Set up material.
    const float DistanceScale = 2900.0f;
    SerializedMaterial matData;
    matData.VertexInputs = Vert::GetVertexAttributes();
    DataLine vIn_Pos(VertexInputNode::GetInstance(), 0),
             vIn_Normal(VertexInputNode::GetInstance(), 1);
    DataNode::Ptr worldPosTemp(new MultiplyNode(vIn_Pos, DistanceScale, "worldPos"));
    DataNode::Ptr worldPos(new AddNode(worldPosTemp, CameraDataNode::GetCamPos(), "objPos"));
    DataNode::Ptr screenPos(new SpaceConverterNode(worldPos,
                                                   SpaceConverterNode::ST_OBJECT,
                                                   SpaceConverterNode::ST_SCREEN,
                                                   SpaceConverterNode::DT_POSITION,
                                                   "screenPos"));
    DataLine screenPos4(screenPos, 1);
    matData.MaterialOuts.VertexPosOutput = screenPos4;
    matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_Pos", vIn_Pos));
    DataLine fIn_ObjPos(FragmentInputNode::GetInstance(), 0);
    DataNode::Ptr cubeTex(new TextureSampleCubemapNode(fIn_ObjPos, "u_cubeTex", "cubeTexNode"));
    DataLine cubeTexRGB(cubeTex, TextureSampleCubemapNode::GetOutputIndex(CO_AllColorChannels));
    DataNode::Ptr outCol(new CombineVectorNode(cubeTexRGB, 1.0f, "outCol"));
    matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color", outCol));
    ShaderGenerator::GeneratedMaterial genM =
        ShaderGenerator::GenerateMaterial(matData, cubeParams, BlendMode::GetOpaque());
    if (!genM.ErrorMessage.empty())
    {
        outError = "Error generating material: " + genM.ErrorMessage;
        return;
    }
    cubeMat.reset(genM.Mat);
    cubeParams["u_cubeTex"].Tex() = tex.GetTextureHandle();


    //Set up mesh.
    std::vector<Vert> verts;
    std::vector<unsigned int> inds;
    PrimitiveGenerator::GenerateCubemapCube(verts, inds, true, true);
    cubeMesh.SetVertexData(verts, Mesh::BUF_STATIC, matData.VertexInputs);
    cubeMesh.SetIndexData(inds, Mesh::BUF_STATIC);
}

void OOS::Render(RenderInfo& worldRendInfo)
{
    cubeMat->Render(cubeMesh, Transform(), worldRendInfo, cubeParams);
}