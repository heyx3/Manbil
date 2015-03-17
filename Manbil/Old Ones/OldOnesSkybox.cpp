#include "OldOnesSkybox.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/Primitives/PrimitiveGenerator.h"
#include "WorldObject.h"


typedef OldOnesSkybox OOS;
typedef PrimitiveGenerator::CubemapVertex Vert;


OOS::OldOnesSkybox(std::string& outError)
    : cubeMat(0),
      tex(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, false)
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
    DataNode::ClearMaterialData();
    DataNode::VertexIns = Vert::GetVertexAttributes();
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
    DataNode::MaterialOuts.VertexPosOutput = screenPos4;
    DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_Pos", vIn_Pos));
    DataLine fIn_ObjPos(FragmentInputNode::GetInstance(), 0);
    DataNode::Ptr cubeTex(new TextureSampleCubemapNode(fIn_ObjPos, "u_cubeTex", "cubeTexNode"));
    DataLine cubeTexRGB(cubeTex, TextureSampleCubemapNode::GetOutputIndex(CO_AllColorChannels));
    DataNode::Ptr outCol(new CombineVectorNode(cubeTexRGB, 1.0f, "outCol"));
    DataNode::MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color", outCol));
    ShaderGenerator::GeneratedMaterial genM =
        ShaderGenerator::GenerateMaterial(cubeParams, BlendMode::GetOpaque());
    if (!genM.ErrorMessage.empty())
    {
        outError = "Error generating material: " + genM.ErrorMessage;
        return;
    }
    cubeMat = genM.Mat;
    cubeParams.TextureCubemaps["u_cubeTex"].Texture = tex.GetTextureHandle();


    //Set up mesh.
    std::vector<Vert> verts;
    std::vector<unsigned int> inds;
    PrimitiveGenerator::GenerateCubemapCube(verts, inds, true, true);
    cubeMesh.SubMeshes.push_back(MeshData(false, PT_TRIANGLE_LIST));
    cubeMesh.SubMeshes[0].SetVertexData(verts, MeshData::BUF_STATIC, DataNode::VertexIns);
    cubeMesh.SubMeshes[0].SetIndexData(inds, MeshData::BUF_STATIC);
}
OOS::~OldOnesSkybox(void)
{
    if (cubeMat != 0)
    {
        delete cubeMat;
    }
}

void OOS::Render(RenderInfo& worldRendInfo)
{
    RenderingState(RenderingState::C_NONE).EnableState();
    cubeMat->Render(worldRendInfo, &cubeMesh, cubeParams);
}