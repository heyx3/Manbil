#include "VoxelWorldPPC.h"

#include "../../Rendering/Materials/Data Nodes/DataNodes.hpp"
#include "../../ScreenClearer.h"


//The PPC's render target may have an error on creation.
std::string errorMsg;

VoxelWorldPPC::VoxelWorldPPC(VoxelWorld& _world, std::string& outError)
    : world(_world), colorTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F, false),
      rendTarg(PS_16U_DEPTH, errorMsg)
{
    //See if there was an error with creating the render target.
    if (!errorMsg.empty())
    {
        outError = "Error initializing render target: " + errorMsg;
        errorMsg.clear();
        return;
    }


    //Set up the render target/color texture to be the size of the game window.
    colorTex.Create();
    Array2D<Vector4b> colorData(world.GetWindow()->getSize().x, world.GetWindow()->getSize().y);
    colorTex.SetColorData(colorData);
    rendTarg.SetColorAttachment(RenderTargetTex(&colorTex), true);

    //Set up the post-process material.

    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetVertexInputData();
    

    //Vertex shader.

    DataLine vIn_Pos(VertexInputNode::GetInstance(), 0),
             vIn_UV(VertexInputNode::GetInstance(), 1);

    //Just pass-through position and UV.
    DataNode::Ptr pos4(new CombineVectorNode(vIn_Pos, 1.0f, "vertPos4"));
    DataNode::MaterialOuts.VertexPosOutput = pos4;
    DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_UV", vIn_UV));


    //Fragment shader.

    DataLine fIn_UV(FragmentInputNode::GetInstance(), 0);

    //Sample and linearize the depth.
    DataNode::Ptr depthTexSampler(new TextureSample2DNode(fIn_UV, "u_depthTex", "depthTexNode"));
    DataLine depthValue(depthTexSampler, TextureSample2DNode::GetOutputIndex(CO_Red));
    DataNode::Ptr linearDepth(new LinearizeDepthSampleNode(depthValue, "linearDepth"));

    //Sample the color.
    DataNode::Ptr colorTexSampler(new TextureSample2DNode(fIn_UV, "u_colorTex", "colorTexNode"));
    DataLine colorValue(colorTexSampler, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));

    //Create fog based on depth.
    DataLine fogColor(Vector3f(1.0f, 1.0f, 1.0f));
    DataNode::Ptr fogLerp(new PowNode(linearDepth, 1.0f, "fogLerpValue"));
    DataNode::Ptr foggedColor(new InterpolateNode(colorValue, fogColor, fogLerp,
                                                  InterpolateNode::IT_Linear, "foggedColor"));

    DataNode::Ptr finalColor(new CombineVectorNode(foggedColor, 1.0f, "finalColor"));
    DataNode::MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color", finalColor));

    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(ppParams,
                                                                                BlendMode::GetOpaque());
    if (!genM.ErrorMessage.empty())
    {
        errorMsg = "Error generating fog shader: " + genM.ErrorMessage;
        return;
    }
    ppMat = genM.Mat;
}
VoxelWorldPPC::~VoxelWorldPPC(void)
{
    delete ppMat;
}

void VoxelWorldPPC::OnWindowResized(unsigned int newW, unsigned int newH)
{
    Array2D<Vector4b> colorData(newW, newH);
    colorTex.SetColorData(colorData);

    rendTarg.UpdateSize();
}

void VoxelWorldPPC::RenderPostProcessing(RenderObjHandle colorIn, RenderObjHandle depthIn,
                                         const ProjectionInfo& pInfo)
{
    //Set up params.
    ppParams.Texture2Ds["u_depthTex"].Texture = depthIn;
    ppParams.Texture2Ds["u_colorTex"].Texture = colorIn;

    //Set up the render info struct.
    Camera cam;
    cam.PerspectiveInfo = pInfo;
    Matrix4f identity;
    RenderInfo info(world.GetTotalElapsedSeconds(), &cam, &identity, &identity);


    //Render.

    rendTarg.EnableDrawingInto();

    ScreenClearer().ClearScreen();
    BlendMode::GetOpaque().EnableMode();
    glViewport(0, 0, colorTex.GetWidth(), colorTex.GetHeight());

    DrawingQuad::GetInstance()->Render(info, ppParams, *ppMat);

    rendTarg.DisableDrawingInto();
}