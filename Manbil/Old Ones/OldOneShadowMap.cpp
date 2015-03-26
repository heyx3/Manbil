#include "OldOneShadowMap.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"



ShaderGenerator::GeneratedMaterial GenMat(bool useUV, bool useNormalMapping)
{
    DataNode::ClearMaterialData();

    DataNode::VertexIns = WorldObject::GetVertexInputs(useUV, useNormalMapping);

    DataLine vIn_Pos = DataLine(VertexInputNode::GetInstance(), 0);

    //Output screen position.
    DataNode::Ptr screenPos = SpaceConverterNode::ObjPosToScreenPos(vIn_Pos, "objPosToScreen");
    DataNode::MaterialOuts.VertexPosOutput = DataLine(screenPos, 1);

    DataNode::Ptr depthOut(new CustomExpressionNode("gl_FragCoord.z", 1, "outDepth"));
    DataNode::MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fragmentDepth", depthOut));

    return ShaderGenerator::GenerateMaterial(UniformDictionary(), BlendMode::GetOpaque());
}

OldOneShadowMap::OldOneShadowMap(std::vector<std::shared_ptr<WorldObject>>& worldObjects,
                                 std::string& err)
    : objs(worldObjects), rt(PS_16U_DEPTH, err),
      matUVNormal(0), matUVNoNormal(0), matNormalNoUV(0), matNoUVNormal(0),
      depthTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_24U_DEPTH, false)
{
    if (!err.empty())
    {
        err = "Error setting up render target: " + err;
        return;
    }

    ShaderGenerator::GeneratedMaterial genM("");

    genM = GenMat(true, true);
    if (!genM.ErrorMessage.empty())
    {
        err = "Error generating uv-normal: " + genM.ErrorMessage;
        return;
    }
    matUVNormal = genM.Mat;
    genM = GenMat(true, false);
    if (!genM.ErrorMessage.empty())
    {
        err = "Error generating uv-noNormal: " + genM.ErrorMessage;
        return;
    }
    matUVNoNormal = genM.Mat;
    genM = GenMat(false, true);
    if (!genM.ErrorMessage.empty())
    {
        err = "Error generating noUv-normal: " + genM.ErrorMessage;
        return;
    }
    matNormalNoUV = genM.Mat;
    genM = GenMat(false, false);
    if (!genM.ErrorMessage.empty())
    {
        err = "Error generating noUv-noNormal: " + genM.ErrorMessage;
        return;
    }
    matNoUVNormal = genM.Mat;


    //Set up render target.
    depthTex.Create();
    depthTex.ClearData(1500, 1500);
    if (!rt.SetDepthAttachment(RenderTargetTex(&depthTex)))
    {
        err = "Error setting depth attachment.";
        return;
    }
    rt.UpdateSize();
}
OldOneShadowMap::~OldOneShadowMap(void)
{
    if (matUVNormal != 0)
    {
        delete matUVNormal;
    }
    if (matUVNoNormal != 0)
    {
        delete matUVNoNormal;
    }
    if (matNormalNoUV != 0)
    {
        delete matNormalNoUV;
    }
    if (matNoUVNormal != 0)
    {
        delete matNoUVNormal;
    }
}

void OldOneShadowMap::Render(float totalSeconds)
{
    const float orthoScale = 250.7f,
                rightMargin = -200.6;
    const Vector3f pos(201.718f, 102.84f, 0.0f);

    Camera cam(pos, WorldObject::LightDir, Vector3f(0.0f, 0.0f, 1.0f));
    cam.MinOrthoBounds = Vector3f(-orthoScale, -orthoScale, -orthoScale);
    cam.MaxOrthoBounds = Vector3f(orthoScale + rightMargin, orthoScale, orthoScale);
    
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);

    RenderInfo info(totalSeconds, &cam, &viewM, &projM);
    
    rt.EnableDrawingInto();
    ScreenClearer(false, true, false).ClearScreen();
    RenderingState(RenderingState::C_NONE).EnableState();
    glViewport(0, 0, depthTex.GetWidth(), depthTex.GetHeight());
    for (unsigned int i = 0; i < objs.size(); ++i)
    {
        if (objs[i]->GetUsesVertexUVs())
        {
            if (objs[i]->GetUsesNormalMaps())
            {
                matUVNormal->Render(info, &objs[i]->MyMesh, UniformDictionary());
            }
            else
            {
                matUVNoNormal->Render(info, &objs[i]->MyMesh, UniformDictionary());
            }
        }
        else
        {
            if (objs[i]->GetUsesNormalMaps())
            {
                matNormalNoUV->Render(info, &objs[i]->MyMesh, UniformDictionary());
            }
            else
            {
                matNoUVNormal->Render(info, &objs[i]->MyMesh, UniformDictionary());
            }
        }
    }
    rt.DisableDrawingInto();
}