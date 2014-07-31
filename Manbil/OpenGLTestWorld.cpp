#include "OpenGLTestWorld.h"


#include <iostream>

#include "Material.h"
#include "ScreenClearer.h"
#include "RenderingState.h"
#include "Input/Input Objects/MouseBoolInput.h"
#include "Math/Higher Math/BumpmapToNormalmap.h"

#include "Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "Math/NoiseGeneration.hpp"
#include "Rendering/GPU Particles/GPUParticleGenerator.h"
#include "Rendering/GPU Particles/High-level GPU Particles/SpecialHGPComponents.h"
#include "Rendering/GUI/TextRenderer.h"
#include "Rendering/PrimitiveGenerator.h"

#include <assert.h>


typedef MaterialConstants MC;


namespace OGLTestPrints
{
	bool PrintRenderError(const char * errorIntro)
{
	const char * error = GetCurrentRenderingError();
	if (strcmp(error, "") != 0)
	{
		std::cout << errorIntro << ": " << error << "\n";
		ClearAllRenderingErrors();
		return false;
	}

	return true;
}

	void Pause(void)
	{
		char dummy;
		std::cin >> dummy;
	}
}
using namespace OGLTestPrints;



Vector2i windowSize(800, 600);
const RenderingState worldRenderState;
std::string texSamplerName = "";
const unsigned int maxRipples = 3,
                   maxFlows = 2;


void OpenGLTestWorld::InitializeTextures(void)
{
    //World render target.

    worldColorTex1.Create();
    worldColorTex1.ClearData(windowSize.x, windowSize.y);
    worldColorTex2.Create();
    worldColorTex2.ClearData(windowSize.x, windowSize.y);
    worldDepthTex.Create();

    worldRenderID = RenderTargets->CreateRenderTarget(PixelSizes::PS_16U_DEPTH);
    if (worldRenderID == RenderTargetManager::ERROR_ID)
    {
        std::cout << "Error creating world render target: " << RenderTargets->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }

    std::vector<RenderTargetTex> colorTargTexes;
    colorTargTexes.insert(colorTargTexes.end(), RenderTargetTex(&worldColorTex1));
    colorTargTexes.insert(colorTargTexes.end(), RenderTargetTex(&worldColorTex2));
    if (!(*RenderTargets)[worldRenderID]->SetDepthAttachment(RenderTargetTex(&worldDepthTex)))
    {
        std::cout << "Error attaching depth texture for world render target: " << (*RenderTargets)[worldRenderID]->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }
    if (!(*RenderTargets)[worldRenderID]->SetColorAttachments(colorTargTexes, true))
    {
        std::cout << "Error attaching color textures for world render target: " << (*RenderTargets)[worldRenderID]->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }


    //Water normal-map creation.
    waterNormalTex.Create();
    std::string error;
    if (!waterNormalTex.SetDataFromFile("Content/Textures/Normalmap.png", error))
    {
        std::cout << "Failed to load 'Content/Textures/Normalmap.png': " + error + "\n";
        Pause();
        EndWorld();
        return;
    }


    //Cubemap creation.
    cubemapTex.Create();
    if (false)
    {
        //Wall textures.
        Array2D<Vector4f> cubemapNegX(2, 2), cubemapNegY(2, 2), cubemapPosX(2, 2), cubemapPosY(2, 2);
        cubemapNegX.FillFunc([](Vector2u loc, Vector4f * outVal) { *outVal = Vector4f(0.25f, 0.25f, (float)loc.y, 1.0f); });
        cubemapNegY.FillFunc([](Vector2u loc, Vector4f * outVal) { *outVal = Vector4f(0.25f, 0.25f, (float)loc.y, 1.0f); });
        cubemapPosX.FillFunc([](Vector2u loc, Vector4f * outVal) { *outVal = Vector4f(0.25f, 0.25f, (float)loc.y, 1.0f); });
        cubemapPosY.FillFunc([](Vector2u loc, Vector4f * outVal) { *outVal = Vector4f(0.25f, 0.25f, (float)loc.y, 1.0f); });
        MTextureCubemap::TransformFaces(cubemapPosX, cubemapNegX, cubemapNegY);

        //Floor/ceiling textures.
        Array2D<Vector4f> cubemapFloor(2, 2, Vector4f(0.25f, 0.25f, 0.0f, 1.0f)),
                          cubemapCeiling(2, 2, Vector4f(0.25f, 0.25f, 1.0f, 0.0f));

        cubemapTex.SetDataColor(cubemapNegX, cubemapNegY, cubemapFloor, cubemapPosX, cubemapPosY, cubemapCeiling);
    }
    else
    {
        error = cubemapTex.SetDataFromFiles("Content/Cubemaps/Sky_Neg_X.png", "Content/Cubemaps/Sky_Neg_Y.png", "Content/Cubemaps/Sky_Neg_Z.png",
                                            "Content/Cubemaps/Sky_Pos_X.png", "Content/Cubemaps/Sky_Pos_Y.png", "Content/Cubemaps/Sky_Pos_Z.png");
        if (!error.empty())
        {
            std::cout << "Error loading cubemap textures: " << error << "\n";
            Pause();
            EndWorld();
            return;
        }
    }

    //3D Texture map creation.
    gsTestTex3D.Create();
    Vector3u size(10, 10, 10);
    Array3D<Vector4f> pixelData(size.x, size.y, size.z);
    pixelData.FillFunc([size](Vector3u loc, Vector4f * outVal)
    {
        *outVal = Vector4f((float)loc.x / (size.x - 1),
                           (float)loc.y / (size.y - 1),
                           (float)loc.z / (size.z - 1),
                           1.0f);
    });
    if (!gsTestTex3D.SetColorData(pixelData, PixelSizes::PS_32F))
    {
        std::cout << "Error setting data for gsTest 3D texture.\n";
        Pause();
        EndWorld();
        return;
    }

    //Set up the test font.
    unsigned int testFontID = TextRender->CreateAFont("Content/Fonts/Candara.ttf", 16);
    if (testFontID == FreeTypeHandler::ERROR_ID)
    {
        std::cout << "Error creating font 'Content/Fonts/Candara.ttf': " << TextRender->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
    if (!TextRender->CreateTextRenderSlots(testFontID, 512, 64, true, TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
    {
        std::cout << "Error creating font slot for 'Content/Fonts/Candara.ttf': " << TextRender->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
    testFontSlot = TextRenderer::FontSlot(testFontID, 0);
    if (!TextRender->RenderString(testFontSlot, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghigjklmopqrstuvwxyz", windowSize.x, windowSize.y))
    {
        std::cout << "Error rendering test string: " << TextRender->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
}
void OpenGLTestWorld::InitializeMaterials(void)
{
    typedef DataNodePtr DNP;
    typedef RenderingChannels RC;


    #pragma region Water


    //Vertex output 0: object-space position.
    //Vertex output 1: UV coords.
    //Vertex output 2: water rand seeds.
    //Vertex output 3: world-space position.

    ShaderInOutAttributes fragInputAttributes(3, 2, 2, 3, false, false, false, false);
    DataLine materialUVs(DNP(new ShaderInNode(2, 1, -1, 0, 1)), 0);
    DNP fragmentInput(new FragmentInputNode(fragInputAttributes));
    DNP waterVertexInput(new VertexInputNode(WaterVertex::GetAttributeData()));
    DNP waterNode(new WaterNode(DataLine(waterVertexInput, 0),
                                DataLine(fragmentInput, 0),
                                3, 2));
    channels[RC::RC_VERTEX_OUT_0] = DataLine(waterNode, WaterNode::GetVertexPosOutputIndex());
    channels[RC::RC_VERTEX_OUT_1] = materialUVs;
    channels[RC::RC_VERTEX_OUT_2] = DataLine(waterVertexInput, 2);
    channels[RC::RC_VERTEX_OUT_3] = DataLine(DNP(new ObjectPosToWorldPosCalcNode(channels[RC::RC_VERTEX_OUT_0])), 0);

    DNP waterSurfaceDistortion(new WaterSurfaceDistortNode(WaterSurfaceDistortNode::GetWaterSeedIn(fragInputAttributes, 2),
                                                           DataLine(0.01f), DataLine(0.5f),
                                                           WaterSurfaceDistortNode::GetTimeIn(fragInputAttributes, 2)));
    DataLine normalMapUVs = DataNodeGenerators::CreateComplexUV(materialUVs,
                                                                DataLine(VectorF(10.0f, 10.0f)),
                                                                DataLine(VectorF(0.0f, 0.0f)),
                                                                DataLine(VectorF(-0.15f, 0.0f)));
    DNP normalMap(new TextureSample2DNode(normalMapUVs, "u_normalMapTex"));
    texSamplerName = ((TextureSample2DNode*)(normalMap.get()))->GetSamplerUniformName();

    DNP finalNormal(new NormalizeNode(DataLine(DNP(new AddNode(DataLine(normalMap, TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels)),
                                                               DataLine(waterNode, WaterNode::GetSurfaceNormalOutputIndex()))), 0)));
    DataLine waterNormal(DNP(new NormalizeNode(DataLine(DNP(new ObjectNormalToWorldNormalCalcNode(DataLine(finalNormal, 0))), 0))), 0);
    DNP light(new LightingNode(DataLine(fragmentInput, 3),
                               waterNormal,//DataLine(DNP(new NormalizeNode(DataLine(DNP(new ObjectNormalToWorldNormalCalcNode(DataLine(finalNormal, 0))), 0))), 0),//TODO: Remove the last outer "Normalize" node; it's already normalized.
                               DataLine(Vector3f(-1, -1, -0.1f).Normalized()),
                               DataLine(0.3f), DataLine(0.7f), DataLine(3.0f), DataLine(256.0f)));

    DNP finalColor(new MultiplyNode(DataLine(light, 0), DataLine(Vector3f(0.275f, 0.275f, 1.0f))));

    channels[RC::RC_Color] = DataLine(DNP(new MultiplyNode(DataLine(light, 0),
                                                           DataLine(Vector3f(0.275f, 0.275f, 1.0f)))), 0);
    channels[RC::RC_VertexPosOutput] = DataLine(DNP(new ObjectPosToScreenPosCalcNode(DataLine(waterNode, WaterNode::GetVertexPosOutputIndex()))),
                                                ObjectPosToScreenPosCalcNode::GetHomogenousPosOutputIndex());
    channels[RC::RC_COLOR_OUT_2] = DataLine(DNP(new CombineVectorNode(DataLine(DNP(new RemapNode(waterNormal, DataLine(-1.0f), DataLine(1.0f))), 0), DataLine(1.0f))), 0);

    UniformDictionary unDict;
    ShaderGenerator::GeneratedMaterial wM = ShaderGenerator::GenerateMaterial(channels, unDict, WaterVertex::GetAttributeData(), RenderingModes::RM_Opaque, true, LightSettings(false));
    if (!wM.ErrorMessage.empty())
    {
        std::cout << "Error generating water shaders: " << wM.ErrorMessage << "\n";
        Pause();
        EndWorld();
        return;
    }
    waterMat = wM.Mat;
    if (waterMat->HasError())
    {
        std::cout << "Error creating water material: " << waterMat->GetErrorMsg() << "\n";
        Pause();
        EndWorld();
        return;
    }


    #pragma endregion


    #pragma region Geometry shader test

    std::unordered_map<RC, DataLine> gsChannels;

    DNP gsTestVertInputs(new VertexInputNode(VertexPos::GetAttributeData()));
    DNP gsTestFragInputs(new FragmentInputNode(ShaderInOutAttributes(2, false)));

    DataLine worldPos(DNP(new ObjectPosToWorldPosCalcNode(DataLine(gsTestVertInputs, 0))), 0);

    DataLine timeTex3D(DNP(new MultiplyNode(DataLine(0.25f), TimeNode::GetTime())), 0);
    DataLine tex3DIn(DNP(new CombineVectorNode(DataLine(gsTestFragInputs, 0), timeTex3D)), 0);
    DataLine tex3DValue(DNP(new TextureSample3DNode(tex3DIn, "u_tex3D")), TextureSample3DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));

    DataLine textSamplerValue(DNP(new TextureSample2DNode(DataLine(gsTestFragInputs, 0), "u_textSampler")), TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_Red));
    textSamplerValue = DataLine(DNP(new CombineVectorNode(textSamplerValue, textSamplerValue, textSamplerValue)), 0);

    gsChannels[RC::RC_VertexPosOutput] = DataLine(DNP(new CombineVectorNode(worldPos, DataLine(VectorF(1.0f)))), 0);
    gsChannels[RC::RC_Color] = DataLine(DNP(new MultiplyNode(textSamplerValue, tex3DValue)), 0);
    
    MaterialUsageFlags geoShaderUsage;
    geoShaderUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_FORWARD);
    geoShaderUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_UPWARDS);
    geoShaderUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_SIDEWAYS);
    geoShaderUsage.EnableFlag(MaterialUsageFlags::DNF_USES_VIEWPROJ_MAT);
    std::string vpTransf = "(" + MC::ViewProjMatName + " * vec4(";
    std::string geoCode = std::string() +
"void main()                                                                                      \n\
{                                                                                                 \n\
    vec3 pos = gl_in[0].gl_Position.xyz;                                                          \n\
    vec3 up = " + MC::CameraUpName + ";                                                           \n\
    vec3 side = " + MC::CameraSideName + ";                                                       \n\
    up = cross(" + MC::CameraForwardName + ", side);                                              \n\
                                                                                                  \n\
    gl_Position = " + vpTransf + "pos + ((u_quadSize.y * up) + (u_quadSize.x * side)), 1.0));     \n\
    UVs = vec2(1.0f, 1.0f);                                                                       \n\
    EmitVertex();                                                                                 \n\
                                                                                                  \n\
    gl_Position = " + vpTransf + "pos + (-(u_quadSize.y * up) + (u_quadSize.x * side)), 1.0));    \n\
    UVs = vec2(1.0f, 0.0f);                                                                       \n\
    EmitVertex();                                                                                 \n\
                                                                                                  \n\
    gl_Position = " + vpTransf + "pos + ((u_quadSize.y * up) - (u_quadSize.x * side)), 1.0));     \n\
    UVs = vec2(0.0f, 1.0f);                                                                       \n\
    EmitVertex();                                                                                 \n\
                                                                                                  \n\
    gl_Position = " + vpTransf + "pos - ((u_quadSize.y * up) + (u_quadSize.x * side)), 1.0));     \n\
    UVs = vec2(0.0f, 0.0f);                                                                       \n\
    EmitVertex();                                                                                 \n\
}";
    
    gsTestParams.FloatUniforms["u_quadSize"] = UniformValueF(ToV2f(TextRender->GetSlotRenderSize(testFontSlot)) * 0.1f, "u_quadSize");
    GeoShaderData geoDat(GeoShaderOutput("UVs", 2), geoShaderUsage, 4, Points, TriangleStrip, gsTestParams, geoCode);
    ShaderGenerator::GeneratedMaterial gsGen = ShaderGenerator::GenerateMaterial(gsChannels, gsTestParams, VertexPos::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false), geoDat);
    if (!gsGen.ErrorMessage.empty())
    {
        std::cout << "Error generating shaders for geometry shader test: " << gsGen.ErrorMessage << "\n";
        Pause();
        EndWorld();
        return;
    }
    gsTestParams.Texture2DUniforms["u_textSampler"].Texture = TextRender->GetRenderedString(testFontSlot)->GetTextureHandle();
    gsTestParams.Texture3DUniforms["u_tex3D"].Texture = gsTestTex3D.GetTextureHandle();
    gsTestMat = gsGen.Mat;


    #pragma endregion


    #pragma region Particles


    std::unordered_map<GPUPOutputs, DataLine> gpupOuts;
    if (false)
    {
        DataLine particleIDInputs(DNP(new ShaderInNode(2, 0, 0, 0, 0)), 0),
                 particleRandSeedInputs(DNP(new ShaderInNode(4, 1, 1, 0, 1)), 0);
        DataLine particleSeed1(DNP(new VectorComponentsNode(particleRandSeedInputs)), 0);
        DataLine elapsedTime(DataNodePtr(new AddNode(particleSeed1, TimeNode::GetTime())), 0);
        DataLine sineTime(DataNodePtr(new SineNode(elapsedTime)), 0);
        DataLine sineTime_0_1(DataNodePtr(new RemapNode(sineTime, DataLine(VectorF(-1.0f)), DataLine(VectorF(1.0f)))), 0);

        gpupOuts[GPUPOutputs::GPUP_WORLDPOSITION] = DataLine(DNP(new AddNode(DataLine(Vector3f(0.0f, 0.0f, 50.0f)),
                                                                             DataLine(DNP(new CombineVectorNode(DataLine(DNP(new MultiplyNode(DataLine(10.0f), particleIDInputs)), 0),
                                                                                                                DataLine(VectorF(0.0f)))), 0))), 0);
        gpupOuts[GPUPOutputs::GPUP_COLOR] = DataLine(DNP(new CombineVectorNode(particleRandSeedInputs, DataLine(1.0f))), 0);
        gpupOuts[GPUPOutputs::GPUP_SIZE] = DataLine(DataNodePtr(new MultiplyNode(DataLine(VectorF(1.0f)),
                                                                                 DataLine(DataNodePtr(new CombineVectorNode(sineTime_0_1, sineTime_0_1)), 0))), 0);
        gpupOuts[GPUPOutputs::GPUP_QUADROTATION] = elapsedTime;
    }
    else
    {
        HGPComponentManager manager(particleParams);
        const unsigned int posSeeds[] = { 5, 1, 3, 2 },
                           velSeeds[] = { 0, 1, 2 },
                           accelSeeds[] = { 3, 4, 5 };
        HGPComponentPtr(3) initialPos(new SpherePositionComponent(manager, Vector3f(500.0f, 500.0f, 50.0f), 20.0f, posSeeds));
        HGPComponentPtr(3) initialVel(new RandomizedHGPComponent<3>(manager, HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(80.0f, 80.0f, 30.0f), manager)),
                                                                    HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(-80.0f, -80.0f, 70.0f), manager)),
                                                                    velSeeds)),
                           accel(new RandomizedHGPComponent<3>(manager, HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(0.0f, 0.0f, -30.0f), manager)),
                                                               HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(0.0f, 0.0f, -90.0f), manager)),
                                                               accelSeeds));
        const unsigned int sizeSeeds[] = { 3, 1 },
                           colorSeeds[] = { 0, 4, 2, 5 };
        manager.SetWorldPosition(HGPComponentPtr(3)(new ConstantAccelerationHGPComponent(manager, accel, initialVel, initialPos)));
        manager.SetSize(HGPComponentPtr(2)(new RandomizedHGPComponent<2>(manager, HGPComponentPtr(2)(new ConstantHGPComponent<2>(VectorF((unsigned int)2, 0.1f), manager)),
                                                                         HGPComponentPtr(2)(new ConstantHGPComponent<2>(VectorF((unsigned int)2, 1.0f), manager)),
                                                                         sizeSeeds)));
        manager.SetColor(HGPComponentPtr(4)(new RandomizedHGPComponent<4>(manager, HGPComponentPtr(4)(new ConstantHGPComponent<4>(VectorF(0.0f, 0.0f, 0.0f, 1.0f), manager)),
                                                                          HGPComponentPtr(4)(new ConstantHGPComponent<4>(VectorF((unsigned int)4, 1.0f), manager)),
                                                                          colorSeeds)));
        manager.SetGPUPOutputs(gpupOuts);
        //gpupOuts[GPUPOutputs::GPUP_COLOR] = DataLine(DataNodePtr(new CombineVectorNode(HGPGlobalData::FifthRandSeed, HGPGlobalData::FifthRandSeed, HGPGlobalData::FifthRandSeed, DataLine(1.0f))), 0);
        manager.Initialize();
    }

    ShaderGenerator::GeneratedMaterial gen = GPUParticleGenerator::GenerateGPUParticleMaterial(gpupOuts, particleParams, RenderingModes::RM_Opaque);
    if (!gen.ErrorMessage.empty())
    {
        std::cout << "Error generating shaders for particle effect: " << gen.ErrorMessage << "\n";
        Pause();
        EndWorld();
        return;
    }
    particleMat = gen.Mat;

    GPUParticleGenerator::NumberOfParticles numb = GPUParticleGenerator::NumberOfParticles::NOP_262144;
    particleMesh.SetVertexIndexData(VertexIndexData(GPUParticleGenerator::GetNumbParticles(numb),
                                                    GPUParticleGenerator::GenerateGPUPParticles(numb)));


    #pragma endregion


    #pragma region Cubemap


    std::unordered_map<RC, DataLine> cubemapChannels;
    DataNodePtr cmVertexInputs(new VertexInputNode(PrimitiveGenerator::CubemapVertex::GetAttributeData()));
    DataNodePtr cmFragInputs(new FragmentInputNode(ShaderInOutAttributes(3, false)));

    DataLine cubemapSampleRGB(DataNodePtr(new TextureSampleCubemapNode(DataLine(cmFragInputs, 0), "u_cubemapTex")),
                              TextureSampleCubemapNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));

    DataLine cmWorldPos(cmVertexInputs, 0);
    cmWorldPos = DataLine(DataNodePtr(new MultiplyNode(cmWorldPos, DataLine(2800.0f))), 0);
    cmWorldPos = DataLine(DataNodePtr(new AddNode(cmWorldPos, CameraDataNode::GetCamPos())), 0);
    cubemapChannels[RC::RC_VertexPosOutput] = DataLine(DataNodePtr(new WorldPosToScreenPosCalcNode(cmWorldPos)),
                                                       WorldPosToScreenPosCalcNode::GetHomogenousPosOutputIndex());

    cubemapChannels[RC::RC_VERTEX_OUT_0] = DataLine(cmVertexInputs, 0);
    cubemapChannels[RC::RC_Color] = DataLine(DataNodePtr(new RemapNode(DataLine(cmFragInputs, 0),
                                                                       DataLine(Vector3f(-1.0f, -1.0f, -1.0f)),
                                                                       DataLine(Vector3f(1.0f, 1.0f, 1.0f)))), 0);
    cubemapChannels[RC::RC_Color] = cubemapSampleRGB;

    ShaderGenerator::GeneratedMaterial cmGen = ShaderGenerator::GenerateMaterial(cubemapChannels, cubemapParams, PrimitiveGenerator::CubemapVertex::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
    if (!cmGen.ErrorMessage.empty())
    {
        std::cout << "Error generating shaders for cubemap material: " << cmGen.ErrorMessage << "\n";
        Pause();
        EndWorld();
        return;
    }
    cubemapParams.TextureCubemapUniforms["u_cubemapTex"].Texture = cubemapTex.GetTextureHandle();
    cubemapMat = cmGen.Mat;


    #pragma endregion


    #pragma region Post-process and final render


    //Post-processing.
    typedef PostProcessEffect::PpePtr PpePtr;
    ppcChain.insert(ppcChain.end(), PpePtr(new FogEffect(DataLine(1.5f), DataLine(Vector3f(1.0f, 1.0f, 1.0f)), DataLine(0.0001f))));


    //Final render.
    finalScreenMatChannels[RC::RC_VertexPosOutput] = DataNodeGenerators::ObjectPosToScreenPos<DrawingQuad>(0);
    finalScreenMatChannels[RC::RC_VERTEX_OUT_1] = DataLine(DNP(new VertexInputNode(DrawingQuad::GetAttributeData())), 1);
    DNP finalTexSampler(new TextureSample2DNode(DataLine(DNP(new FragmentInputNode(DrawingQuad::GetAttributeData())), 1), "u_finalRenderSample"));
    finalScreenMatChannels[RC::RC_Color] = DataLine(finalTexSampler, TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    UniformDictionary uniformDict;
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(finalScreenMatChannels, uniformDict, DrawingQuad::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
    if (!genM.ErrorMessage.empty())
    {
        std::cout << "Error generating shaders for final screen material: " << genM.ErrorMessage << "\n";
        Pause();
        EndWorld();
        return;
    }
    finalScreenMat = genM.Mat;
    if (finalScreenMat->HasError())
    {
        std::cout << "final screen material creation error: " << finalScreenMat->GetErrorMsg() << "\n";
        Pause();
        EndWorld();
        return;
    }

    finalScreenQuad = new DrawingQuad();
    finalScreenQuadParams = uniformDict;


    #pragma endregion
}
void OpenGLTestWorld::InitializeObjects(void)
{
    //Set up geometry shader mesh.
    RenderObjHandle gsVBO;
    Vector3f vertex[1] = { Vector3f(0.0f, 0.0f, 0.0f) };
    RenderDataHandler::CreateVertexBuffer(gsVBO, &vertex, 1, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    gsMesh.SetVertexIndexData(VertexIndexData(1, gsVBO));
    gsMesh.Transform.SetPosition(Vector3f(90.0f, 90.0f, 90.0f));


    //Water.

    const unsigned int size = 300;

    water = new Water(size, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(6.0f, 6.0f, 2.0f),
                      OptionalValue<Water::RippleWaterCreationArgs>(Water::RippleWaterCreationArgs(maxRipples)),
                      OptionalValue<Water::DirectionalWaterCreationArgs>(Water::DirectionalWaterCreationArgs(maxFlows)),
                      OptionalValue<Water::SeedmapWaterCreationArgs>());
    water->GetTransform().IncrementPosition(Vector3f(0.0f, 0.0f, -10.0f));

    water->UpdateUniformLocations(waterMat);
    water->Params.Texture2DUniforms[texSamplerName] =
        UniformSampler2DValue(waterNormalTex.GetTextureHandle(), texSamplerName,
                            waterMat->GetUniforms(RenderPasses::BaseComponents).FindUniform(texSamplerName, waterMat->GetUniforms(RenderPasses::BaseComponents).Texture2DUniforms).Loc);
    //TODO: Try changing the above line to just use "... .Texture = waterNormalTex.GetTextureHandle()". Look for similiar issues in other worlds.

    water->AddFlow(Water::DirectionalWaterArgs(Vector2f(2.0f, 0.0f), 10.0f, 50.0f));


    //Cubemap.

    std::vector<PrimitiveGenerator::CubemapVertex> cmVertices;
    std::vector<unsigned int> cmIndices;
    PrimitiveGenerator::GenerateCubemapCube(cmVertices, cmIndices, false, true);

    RenderObjHandle cmVBO, cmIBO;
    RenderDataHandler::CreateVertexBuffer(cmVBO, cmVertices.data(), cmVertices.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(cmIBO, cmIndices.data(), cmIndices.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    cubemapMesh.SetVertexIndexData(VertexIndexData(cmVertices.size(), cmVBO, cmIndices.size(), cmIBO));


    //Post-process chain.
    ppc = new PostProcessChain(ppcChain, windowSize.x, windowSize.y, false,
                               TextureSampleSettings2D(FT_NEAREST, WT_CLAMP),
                               PixelSizes::PS_32F, *RenderTargets);
    if (ppc->HasError())
    {
        std::cout << "Error creating post-process chain: " << ppc->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
}


OpenGLTestWorld::OpenGLTestWorld(void)
    : SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(24, 0, 0, 4, 1)),
      water(0), ppc(0), finalScreenQuad(0), finalScreenMat(0),
      gsTestMat(0), gsMesh(PrimitiveTypes::Points),
      particleMat(0), particleMesh(PrimitiveTypes::Points),
      particleManager(particleParams),
      cubemapMesh(PrimitiveTypes::TriangleList), cubemapMat(0),
      gsTestTex3D(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PS_32F, false),
      waterNormalTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PS_32F, true),
      cubemapTex(TextureSampleSettings3D(FT_LINEAR, WT_CLAMP), PS_32F, true),
      worldColorTex1(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F, false),
      worldColorTex2(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F, false),
      worldDepthTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F_DEPTH, false)
{
}
void OpenGLTestWorld::InitializeWorld(void)
{
	SFMLOpenGLWorld::InitializeWorld();
	if (IsGameOver()) return;

    std::string err = InitializeStaticSystems(true, true, true);
    if (!err.empty())
    {
        std::cout << "Error initializing systems: " << err << "\n";
        Pause();
        EndWorld();
        return;
    }
	

    Input.AddBoolInput(666, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Button::Left, BoolInput::ValueStates::JustPressed)));


	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();


    Vector3f pos(2.0f, 2.0f, 10.0f);
    cam.SetPosition(pos);
    cam.SetRotation(-pos, Vector3f(0.0f, 0.0f, 1.0f), false);
    cam.Window = GetWindow();
    cam.Info.FOV = ToRadian(55.0f);
    cam.Info.zFar = 5000.0f;
    cam.Info.zNear = 1.0f;
    cam.Info.Width = windowSize.x;
    cam.Info.Height = windowSize.y;
    cam.SetMoveSpeed(100.0f);
    cam.SetRotSpeed(0.25f);
}

OpenGLTestWorld::~OpenGLTestWorld(void)
{
    DeleteAndSetToNull(water);
    DeleteAndSetToNull(waterMat);
    DeleteAndSetToNull(ppc);
    DeleteAndSetToNull(finalScreenQuad);
    DeleteAndSetToNull(finalScreenMat);
    DeleteAndSetToNull(particleMat);
    DeleteAndSetToNull(cubemapMat);
}
void OpenGLTestWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(water);
    DeleteAndSetToNull(waterMat);
    DeleteAndSetToNull(ppc);
    DeleteAndSetToNull(finalScreenQuad);
    DeleteAndSetToNull(finalScreenMat);
    DeleteAndSetToNull(particleMat);
    DeleteAndSetToNull(cubemapMat);
    waterNormalTex.DeleteIfValid();
    cubemapTex.DeleteIfValid();
    gsTestTex3D.DeleteIfValid();
}

void OpenGLTestWorld::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	std::cout << "Enter any key to continue:\n";
	Pause();
}


void OpenGLTestWorld::UpdateWorld(float elapsedSeconds)
{
	if (cam.Update(elapsedSeconds))
	{
		EndWorld();
	}

    water->Update(elapsedSeconds);
    particleManager.Update(elapsedSeconds);
    if (particleParams.FloatUniforms[HGPGlobalData::ParticleElapsedTimeUniformName].Value[0] >= 5.0f)
        particleParams.FloatUniforms[HGPGlobalData::ParticleElapsedTimeUniformName].Value[0] -= 5.0f;

    if (Input.GetBoolInputValue(666))
        water->AddRipple(Water::RippleWaterArgs(cam.GetPosition(), 5000.0f, 40.0f, 120.0f, 4.0f));
}

void OpenGLTestWorld::RenderWorldGeometry(const RenderInfo & info)
{
    ScreenClearer().ClearScreen();
    worldRenderState.EnableState();

    std::vector<const Mesh*> meshList;

    //Water.
    meshList.insert(meshList.end(), &water->GetMesh());
    if (!waterMat->Render(RenderPasses::BaseComponents, info, meshList, water->Params))
    {
        std::cout << "Error rendering water: " << waterMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    //Geometry shader test.
    meshList.clear();
    meshList.insert(meshList.end(), &gsMesh);
    if (!gsTestMat->Render(RenderPasses::BaseComponents, info, meshList, gsTestParams))
    {
        std::cout << "Error rendering geometry shader test: " << gsTestMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    //Particles.
    meshList.clear();
    meshList.insert(meshList.end(), &particleMesh);
    if (!particleMat->Render(RenderPasses::BaseComponents, info, meshList, particleParams))
    {
        std::cout << "Error rendering particles: " << particleMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    //Cubemap.
    meshList.clear();
    meshList.insert(meshList.end(), &cubemapMesh);
    if (!cubemapMat->Render(RenderPasses::BaseComponents, info, meshList, cubemapParams))
    {
        std::cout << "Error rendering cubemap: " << cubemapMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }
}

void OpenGLTestWorld::RenderOpenGL(float elapsedSeconds)
{
	Matrix4f worldM, viewM, projM;
	TransformObject dummy;

	worldM.SetAsIdentity();
	cam.GetViewTransform(viewM);
	projM.SetAsPerspProj(cam.Info);
    //cam.GetOrthoProjection(projM);

    //Render the world into a render target.
    RenderInfo info((SFMLOpenGLWorld*)this, (Camera*)&cam, &dummy, &worldM, &viewM, &projM);
    (*RenderTargets)[worldRenderID]->EnableDrawingInto();
    RenderWorldGeometry(info);
    (*RenderTargets)[worldRenderID]->DisableDrawingInto(windowSize.x, windowSize.y, true);

    //Catch any general errors.
    std::string err = GetCurrentRenderingError();
    if (!err.empty())
    {
        std::cout << "Error rendering world geometry: " << err << "\n";
        Pause();
        EndWorld();
        return;
    }

    //Choose which render color target to use as the world render.
    RenderObjHandle worldRendTex;
    bool useSpecial = sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

    if (useSpecial)
        worldRendTex = (*RenderTargets)[worldRenderID]->GetColorTextures()[1].MTex->GetTextureHandle();
    else worldRendTex = (*RenderTargets)[worldRenderID]->GetColorTextures()[0].MTex->GetTextureHandle();

    //Render post-process effects on top of the world.
    if (!ppc->RenderChain(this, cam.Info, worldRendTex, (*RenderTargets)[worldRenderID]->GetDepthTexture().MTex->GetTextureHandle()))
    {
        std::cout << "Error rendering post-process chain: " << ppc->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }


    //Render the final image.
    ScreenClearer().ClearScreen();
    Camera cam;
    TransformObject trans;
    Matrix4f identity;
    identity.SetAsIdentity();
    RenderTarget * finalRend = ppc->GetFinalRender();
    RenderObjHandle finalRendTex;
    if (finalRend == 0)
    {
        finalRendTex = (*RenderTargets)[worldRenderID]->GetColorTextures()[useSpecial ? 1 : 0].MTex->GetTextureHandle();
    }
    else finalRendTex = finalRend->GetColorTextures()[0].MTex->GetTextureHandle();
    finalScreenQuadParams.Texture2DUniforms["u_finalRenderSample"].Texture = finalRendTex;
    if (!finalScreenQuad->Render(RenderPasses::BaseComponents, RenderInfo(this, &cam, &trans, &identity, &identity, &identity), finalScreenQuadParams, *finalScreenMat))
    {
        std::cout << "Error rendering final screen output: " << finalScreenMat->GetErrorMsg() << "\n";
        Pause();
        EndWorld();
        return;
    }
}


void OpenGLTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
	ClearAllRenderingErrors();

	glViewport(0, 0, newW, newH);
	cam.Info.Width = (float)newW;
	cam.Info.Height = (float)newH;
    windowSize.x = newW;
    windowSize.y = newH;
    worldColorTex1.ClearData(newW, newH);
    worldColorTex2.ClearData(newW, newH);
    if (!(*RenderTargets)[worldRenderID]->UpdateSize())
    {
        std::cout << "Error resizing world render target: " << (*RenderTargets)[worldRenderID]->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }
    if (!ppc->ResizeRenderTargets(newW, newH))
    {
        std::cout << "Error resizing PPC render target: " << ppc->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
}