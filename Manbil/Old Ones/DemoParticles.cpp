#include "DemoParticles.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/GPU Particles/GPUParticleNodes.h"
#include "../Rendering/GPU Particles/GPUParticleGenerator.h"


const std::string texUniformName = "u_tex";
const std::string basePosUniformName = "u_basePos";
const std::string elapsedTimeUniformName = "u_particleTime";

const float appearanceParticlesLength = 8.0f;


DemoParticles::DemoParticles(FractalRenderer& _oldOne, std::string& err)
    : smokeTex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_8U, true),
      oldOne(_oldOne)
{
    //Load the textures.
    smokeTex.Create();
    if (!smokeTex.SetDataFromFile("Content/Old Ones/Particles/Smoke.jpg", err))
    {
        err = "Error loading 'Smoke.jpg' texture: " + err;
        return;
    }


    DataNode::Ptr texSamplerPtr(new TextureSample2DNode(GPUParticleGenerator::FragmentShaderUV,
                                                        texUniformName, "texSampler"));
    DataLine texRGB(texSamplerPtr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels)),
             texRGBA(texSamplerPtr, TextureSample2DNode::GetOutputIndex(CO_AllChannels));
    DataNode::Ptr time(new ParamNode(1, elapsedTimeUniformName, "timeN"));


    //Create the "appearance" particles.

    DataNode::Ptr randRot(new InterpolateNode(0.0f, 360.0f, GetRandSeedFloat(0),
                                              InterpolateNode::IT_Linear, "randRot"));
    DataNode::Ptr basePos(new ParamNode(3, basePosUniformName, "basePos"));
    DataNode::Ptr velBase(new CombineVectorNode(GetRandSeedFloat(1), GetRandSeedFloat(2),
                                                GetRandSeedFloat(3), "velBase")),
                  velNorm(new CustomExpressionNode("normalize(-1.0 + (2.0 * '0'))", 3, velBase, "velNorm")),
                  vel(new MultiplyNode(velNorm, 5.0f, "vel"));
    DataNode::Ptr pos(new ConstAccelNode(basePos, vel, Vector3f(0.0f, 0.0f, 1.0f) * -0.5f, time));
    DataNode::Ptr timeLerp(new DivideNode(time, appearanceParticlesLength, "timeLerp"));
    DataNode::Ptr col(new InterpolateNode(texRGBA, Vector4f(0.0f, 0.0f, 0.0f, 0.0f), timeLerp,
                                          InterpolateNode::IT_Linear, "colOverTime"));

    std::unordered_map<GPUPOutputs, DataLine> outputs;
    outputs[GPUP_COLOR] = col;
    outputs[GPUP_SIZE] = Vector2f(1.0f, 1.0f) * 0.5f;
    outputs[GPUP_QUADROTATION] = randRot;
    outputs[GPUP_WORLDPOSITION] = pos;

    ShaderGenerator::GeneratedMaterial genM =
        GPUParticleGenerator::GenerateMaterial(outputs, oldOneAppearParams, BlendMode::GetAdditive());
    if (!genM.ErrorMessage.empty())
    {
        err = "Error generating 'appearance' particles: " + genM.ErrorMessage;
        return;
    }
    oldOneAppearParticles = std::shared_ptr<Material>(genM.Mat);

    oldOneAppearParams.Texture2Ds[texUniformName].Texture = smokeTex.GetTextureHandle();

    oldOneAppearMesh.SubMeshes.push_back(MeshData(false, PrimitiveTypes::PT_POINTS));
    MeshData& datAppear = oldOneAppearMesh.SubMeshes[0];
    GPUParticleGenerator::GenerateGPUPParticles(datAppear, GPUParticleGenerator::NOP_1024);


    //Create the "ambient" particles.
    
    DataNode::Ptr outPos(new CustomExpressionNode("'0' + ('2' * mod('1' * '3' * 10.0, 20.0))", 3,
                                                  basePos, time, velNorm, GetRandSeedFloat(5),
                                                  "ambientPos"));
    outputs[GPUP_COLOR] = texRGBA;
    outputs[GPUP_SIZE] = Vector2f(1.0f, 1.0f) * 0.025f;
    outputs[GPUP_WORLDPOSITION] = outPos;
    
    genM = GPUParticleGenerator::GenerateMaterial(outputs, oldOneAmbientParams,
                                                  BlendMode::GetAdditive());
    if (!genM.ErrorMessage.empty())
    {
        err = "Error generating 'ambient' particles: " + genM.ErrorMessage;
        return;
    }
    oldOneAmbientParticles = std::shared_ptr<Material>(genM.Mat);

    oldOneAmbientParams.Texture2Ds[texUniformName].Texture = smokeTex.GetTextureHandle();

    oldOneAmbientMesh.SubMeshes.push_back(MeshData(false, PrimitiveTypes::PT_POINTS));
    MeshData& datAmbient = oldOneAmbientMesh.SubMeshes[0];
    GPUParticleGenerator::GenerateGPUPParticles(datAmbient, GPUParticleGenerator::NOP_16384);
}

DemoParticles::~DemoParticles(void)
{

}


void DemoParticles::Update(float elapsedTime)
{
    totalTime += elapsedTime;

    SetBasePos(oldOne.GetFractalPos());
    SetElapsedTime(totalTime - FractalRenderer::AppearTime);
}
void DemoParticles::Render(const RenderInfo& info)
{
    RenderingState rs(RenderingState::C_NONE, true, false);
    rs.EnableDepthTestState();
    rs.EnableDepthWriteState();

    Matrix4f identity;
    if (totalTime >= FractalRenderer::AppearTime)
    {
        if (totalTime < appearanceParticlesLength + FractalRenderer::AppearTime)
        {
            oldOneAppearParticles->Render(info, oldOneAppearMesh.SubMeshes[0], identity,
                                          oldOneAppearParams);
        }

        oldOneAmbientParticles->Render(info, oldOneAmbientMesh.SubMeshes[0], identity,
                                       oldOneAmbientParams);
    }
}


void DemoParticles::SetBasePos(Vector3f newPos)
{
    oldOneAppearParams.Floats[basePosUniformName].SetValue(newPos);
    oldOneAmbientParams.Floats[basePosUniformName].SetValue(newPos);
}
void DemoParticles::SetElapsedTime(float newTime)
{
    oldOneAppearParams.Floats[elapsedTimeUniformName].SetValue(newTime);
    oldOneAmbientParams.Floats[elapsedTimeUniformName].SetValue(newTime);
}