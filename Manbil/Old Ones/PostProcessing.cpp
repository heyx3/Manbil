#include "PostProcessing.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/Primitives/DrawingQuad.h"


const char *colTexUniformName = "u_colTex",
           *depthTexUniformName = "u_depthTex";

PostProcessing::PostProcessing(Vector2u windowSize, std::string& err)
    : rt1(0), rt2(0),
      col1(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_16U, false),
      col2(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_16U, false)
{
    const int nEffectMaterials = 1;

    for (unsigned int i = 0; i < nEffectMaterials; ++i)
    {
        //Vertex shader and beginning of fragment shader will generally be the same
        //    for every post-process pass.

        DataNode::ClearMaterialData();
        DataNode::VertexIns = DrawingQuad::GetVertexInputData();

        DataLine vIn_Pos(VertexInputNode::GetInstance(), 0),
                 vIn_UV(VertexInputNode::GetInstance(), 1);
        DataNode::Ptr vOut_Pos(new CombineVectorNode(vIn_Pos, 1.0f, "vOutPos"));
        DataNode::MaterialOuts.VertexPosOutput = vOut_Pos;
        DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_UV", vIn_UV));

        DataLine fIn_UV(FragmentInputNode::GetInstance(), 0);
        DataNode::Ptr colSampler(new TextureSample2DNode(fIn_UV, colTexUniformName, "colTex")),
                      depthSampler(new TextureSample2DNode(fIn_UV, depthTexUniformName, "depthTex"));
        DataLine worldRGB(colSampler, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels)),
                 worldDepth(depthSampler, TextureSample2DNode::GetOutputIndex(CO_Red));
        DataNode::Ptr linearDepth(new LinearizeDepthSampleNode(worldDepth, "linearDepth"));


        //Do something different for the fragment shader in each material pass.
        std::vector<DataNode::Ptr> ptrStorage;
        DataLine finalColor;
        switch (i)
        {
            case 0:
            {
                const Vector3f fogCol(0.8f, 0.8f, 0.8f);
                ptrStorage.push_back(DataNode::Ptr(new InterpolateNode(worldRGB, fogCol, linearDepth,
                                                                       InterpolateNode::IT_Linear,
                                                                       "fogged")));
                finalColor = ptrStorage[0];
                break;
            }
            default:
                assert(false);
                err = "PP Material " + std::to_string(i) + " has no code!";
                return;
        }

        //Finalize the material.
        DataNode::Ptr finalRGBA(new CombineVectorNode(finalColor, 1.0f, "finalColor"));
        DataNode::MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color", finalRGBA));

        //Compile the material.
        ppParams.push_back(UniformDictionary());
        ShaderGenerator::GeneratedMaterial genM =
            ShaderGenerator::GenerateMaterial(ppParams[i], BlendMode::GetTransparent());
        if (!genM.ErrorMessage.empty())
        {
            err = genM.ErrorMessage;
            return;
        }
        ppMats.push_back(std::shared_ptr<Material>(genM.Mat));
    }

    //Set up the render targets.
    col1.Create();
    col1.ClearData(windowSize.x, windowSize.y);
    col2.Create();
    col2.ClearData(windowSize.x, windowSize.y);
    rt1 = new RenderTarget(PixelSizes::PS_16U_DEPTH, err);
    if (!err.empty())
    {
        err = "Error setting up first render target: " + err;
        return;
    }
    rt1->SetColorAttachment(RenderTargetTex(&col1), true);
    rt1->UpdateSize();
    rt2 = new RenderTarget(PixelSizes::PS_16U_DEPTH, err);
    if (!err.empty())
    {
        err = "Error setting up second render target: " + err;
        return;
    }
    rt2->SetColorAttachment(RenderTargetTex(&col2), true);
    rt2->UpdateSize();
}
PostProcessing::~PostProcessing(void)
{
    if (rt1 != 0)
    {
        delete rt1;
    }
    if (rt2 != 0)
    {
        delete rt2;
    }

    col1.DeleteIfValid();
    col2.DeleteIfValid();
}

RenderObjHandle PostProcessing::Render(float seconds, ProjectionInfo projInfo,
                                       RenderObjHandle col, RenderObjHandle depth)
{
    RenderingState(RenderingState::C_NONE, false, false).EnableState();

    Camera dummyCam;
    dummyCam.PerspectiveInfo = projInfo;
    Matrix4f identity;
    RenderInfo info(seconds, &dummyCam, &identity, &identity);

    RenderTarget *toUse = 0;

    for (unsigned int i = 0; i < ppMats.size(); ++i)
    {
        //Set up the render target to use and textures to read from.
        ppParams[i].Texture2Ds[depthTexUniformName].Texture = depth;
        if (toUse == 0)
        {
            toUse = rt1;
            ppParams[i].Texture2Ds[colTexUniformName].Texture = col;
        }
        else if (toUse == rt1)
        {
            toUse = rt2;
            ppParams[i].Texture2Ds[colTexUniformName].Texture =
                rt1->GetColorTextures()[0].MTex->GetTextureHandle();
        }
        else
        {
            toUse = rt1;
            ppParams[i].Texture2Ds[colTexUniformName].Texture =
                rt2->GetColorTextures()[0].MTex->GetTextureHandle();
        }

        //Render the post-process material.
        toUse->EnableDrawingInto();
        ScreenClearer(true, false, false, Vector4f(1.0f, 0.0f, 1.0f, 0.0f)).ClearScreen();
        DrawingQuad::GetInstance()->Render(info, ppParams[i], *ppMats[i]);
        toUse->DisableDrawingInto();
    }

    return (toUse == 0 ? col : toUse->GetColorTextures()[0].MTex->GetTextureHandle());
}

void PostProcessing::Resize(Vector2u newWindowSize)
{
    col1.ClearData(newWindowSize.x, newWindowSize.y);
    col2.ClearData(newWindowSize.x, newWindowSize.y);
    rt1->UpdateSize();
    rt2->UpdateSize();
}