#include "OpenGLTestWorld.h"


#include <iostream>

#include "Material.h"
#include "ScreenClearer.h"
#include "RenderingState.h"
#include "TextureSettings.h"
#include "Input/Input Objects/MouseBoolInput.h"
#include "Math/Higher Math/BumpmapToNormalmap.h"

#include "Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "Math/NoiseGeneration.hpp"
#include "Rendering/GPU Particles/GPUParticleGenerator.h"
#include "Rendering/GPU Particles/High-level GPU Particles/SpecialHGPComponents.h"
#include "Rendering/GUI/TextRenderer.h"

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


Vector2i windowSize(250, 250);
const RenderingState worldRenderState;
std::string texSamplerName = "";
const unsigned int maxRipples = 3,
                   maxFlows = 2;


void OpenGLTestWorld::InitializeTextures(void)
{
    //Render target texture settings.
    RendTargetColorTexSettings cts;
    cts.ColorAttachment = 0;
    cts.Settings.Width = windowSize.x;
    cts.Settings.Height = windowSize.y;
    cts.Settings.Size = ColorTextureSettings::CTS_32;
    cts.Settings.Settings = TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false);
    RendTargetDepthTexSettings dts;
    dts.UsesDepthTexture = true;
    dts.Settings.Size = DepthTextureSettings::DTS_24;
    dts.Settings.Settings = TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false);

    //Render target creation.
    worldRenderID = manager.CreateRenderTarget(cts, dts);
    if (worldRenderID == RenderTargetManager::ERROR_ID)
    {
        std::cout << "Error creating world render target: " << manager.GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }

    if (!waterNormalTex.loadFromFile("Content/Textures/Normalmap.png"))
    {
        std::cout << "Failed to load 'Normalmap.png'.\n";
        Pause();
        EndWorld();
        return;
    }
    sf::Texture::bind(&waterNormalTex);
    TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP, true).SetData();


    //Set up the test font.

    testFontID = TextRender->CreateTextRenderSlot("Content/Fonts/Candara.ttf",
                                                  TextureSettings(TextureSettings::TF_LINEAR, TextureSettings::TW_CLAMP, false), 100);
    if (testFontID == FreeTypeHandler::ERROR_ID)
    {
        std::cout << "Error creating font render slot for 'Content/Fonts/Candara.ttf': " << TextRender->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
    if (!TextRender->RenderString(testFontID, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", windowSize.x, windowSize.y))
    {
        std::cout << "Error rendering test string: " << TextRender->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
    //testFontID = FreeTypeHandler::Instance.LoadFont("Content/Fonts/Candara.ttf", FontSizeData(2, 2, 72, 72));
    //if (testFontID == FreeTypeHandler::ERROR_ID)
    //{
    //    std::cout << "Error loading 'Content/Fonts/Candara.ttf': " << FreeTypeHandler::Instance.GetError() << "\n";
    //    Pause();
    //    EndWorld();
    //    return;
    //}
    //if (!FreeTypeHandler::Instance.SetFontSize(testFontID, 300))
    //{
    //    std::cout << "Error setting test font to a new size: " << FreeTypeHandler::Instance.GetError() << "\n";
    //}
    //if (!FreeTypeHandler::Instance.RenderChar(testFontID, '~'))
    //{
    //    std::cout << "Error rendering 'A' using test font: " << FreeTypeHandler::Instance.GetError() << "\n";
    //    Pause();
    //    EndWorld();
    //    return;
    //}

    //sf::Image tempImg;
    //TextureConverters::ToImage(FreeTypeHandler::Instance.GetChar(), tempImg);
    //testFontTex.loadFromImage(tempImg);
    //sf::Texture::bind(&testFontTex);
    //TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false).SetData();
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

    VertexAttributes fragInputAttributes(3, 2, 2, 3, false, false, false, false);
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
    DNP normalMap(new TextureSampleNode(normalMapUVs, "u_normalMapTex"));
    texSamplerName = ((TextureSampleNode*)(normalMap.get()))->GetSamplerUniformName();

    DNP finalNormal(new NormalizeNode(DataLine(DNP(new AddNode(DataLine(normalMap, TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels)),
                                                               DataLine(waterNode, WaterNode::GetSurfaceNormalOutputIndex()))), 0)));

    DNP light(new LightingNode(DataLine(fragmentInput, 3),
                               DataLine(DNP(new NormalizeNode(DataLine(DNP(new ObjectNormalToWorldNormalCalcNode(DataLine(finalNormal, 0))), 0))), 0),//TODO: Remove the last outer "Normalize" node; it's already normalized.
                               DataLine(Vector3f(-1, -1, -0.1f).Normalized()),
                               DataLine(0.3f), DataLine(0.7f), DataLine(3.0f), DataLine(256.0f)));

    DNP finalColor(new MultiplyNode(DataLine(light, 0), DataLine(Vector3f(0.275f, 0.275f, 1.0f))));

    channels[RC::RC_Color] = DataLine(DNP(new MultiplyNode(DataLine(light, 0),
                                                           DataLine(Vector3f(0.275f, 0.275f, 1.0f)))), 0);
    channels[RC::RC_VertexPosOutput] = DataLine(DNP(new ObjectPosToScreenPosCalcNode(DataLine(waterNode, WaterNode::GetVertexPosOutputIndex()))),
                                                ObjectPosToScreenPosCalcNode::GetHomogenousPosOutputIndex());

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
    DataLine worldPos(DNP(new ObjectPosToWorldPosCalcNode(DataLine(DNP(new VertexInputNode(VertexPos::GetAttributeData())), 0))), 0);
    gsChannels[RC::RC_VertexPosOutput] = DataLine(DNP(new CombineVectorNode(worldPos, DataLine(VectorF(1.0f)))), 0);
    gsChannels[RC::RC_Color] = DataLine(DNP(new TextureSampleNode(DataLine(DNP(new FragmentInputNode(VertexAttributes(2, false))), 0), "u_textSampler")),
                                        TextureSampleNode::GetOutputIndex(ChannelsOut::CO_Red));
    gsChannels[RC::RC_Color] = DataLine(DNP(new CombineVectorNode(gsChannels[RC::RC_Color], gsChannels[RC::RC_Color], gsChannels[RC::RC_Color])), 0);
    
    MaterialUsageFlags geoShaderUsage;
    geoShaderUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_FORWARD);
    geoShaderUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_UPWARDS);
    geoShaderUsage.EnableFlag(MaterialUsageFlags::DNF_USES_CAM_SIDEWAYS);
    geoShaderUsage.EnableFlag(MaterialUsageFlags::DNF_USES_VIEWPROJ_MAT);
    std::string vpTransf = "(" + MC::ViewProjMatName + " * vec4(";
    std::string geoCode = std::string() +
"void main()                                                                        \n\
{                                                                                   \n\
    const vec2 size = vec2(2048.0f, 512.0f) * 0.01f;                                \n\
    vec3 pos = gl_in[0].gl_Position.xyz;                                            \n\
    vec3 up = " + MC::CameraUpName + ";                                             \n\
    vec3 side = " + MC::CameraSideName + ";                                         \n\
    up = cross(" + MC::CameraForwardName + ", side);                                \n\
                                                                                    \n\
    gl_Position = " + vpTransf + "pos + ((size.y * up) + (size.x * side)), 1.0));  \n\
    UVs = vec2(1.0f, 1.0f);                                                         \n\
    EmitVertex();                                                                   \n\
                                                                                    \n\
    gl_Position = " + vpTransf + "pos + (-(size.y * up) + (size.x * side)), 1.0));  \n\
    UVs = vec2(1.0f, 0.0f);                                                         \n\
    EmitVertex();                                                                   \n\
                                                                                    \n\
    gl_Position = " + vpTransf + "pos + ((size.y * up) - (size.x * side)), 1.0));   \n\
    UVs = vec2(0.0f, 1.0f);                                                         \n\
    EmitVertex();                                                                   \n\
                                                                                    \n\
    gl_Position = " + vpTransf + "pos - ((size.y * up) + (size.x * side)), 1.0));   \n\
    UVs = vec2(0.0f, 0.0f);                                                         \n\
    EmitVertex();                                                                   \n\
}";
    GeoShaderData geoDat(GeoShaderOutput("UVs", 2), geoShaderUsage, 4, Points, TriangleStrip, gsTestParams, geoCode);
    ShaderGenerator::GeneratedMaterial gsGen = ShaderGenerator::GenerateMaterial(gsChannels, gsTestParams, VertexPos::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false), geoDat);
    if (!gsGen.ErrorMessage.empty())
    {
        std::cout << "Error generating shaders for geometry shader test: " << gsGen.ErrorMessage << "\n";
        Pause();
        EndWorld();
        return;
    }
    gsTestParams.TextureUniforms["u_textSampler"].Texture.SetData(TextRender->GetRenderedString(testFontID));
    gsTestMat = gsGen.Mat;


    #pragma endregion


    #pragma region Particles


    std::unordered_map<GPUPOutputs, DataLine> gpupOuts;
    if (false)
    {
        DataLine particleIDInputs(DNP(new ShaderInNode(2, 0, 0, 0, 0)), 0),
                 particleRandSeedInputs(DNP(new ShaderInNode(4, 1, 1, 0, 1)), 0);
        DataLine particleSeed1(DNP(new VectorComponentsNode(particleRandSeedInputs)), 0);
        DataLine elapsedTime(DataNodePtr(new AddNode(particleSeed1, DataLine(DataNodePtr(new TimeNode()), 0))), 0);
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
        HGPComponentManager manager(Textures, particleParams);
        const unsigned int posSeeds[] = { 5, 1, 3, 2 },
                           velSeeds[] = { 0, 1, 2 },
                           accelSeeds[] = { 3, 4, 5 };
        HGPComponentPtr(3) initialPos(new SpherePositionComponent(manager, Vector3f(0.0f, 0.0f, 50.0f), 20.0f, posSeeds));
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


    #pragma region Post-process and final render


    //Post-processing.
    typedef PostProcessEffect::PpePtr PpePtr;
    ppcChain.insert(ppcChain.end(), PpePtr(new FogEffect(DataLine(1.5f), DataLine(Vector3f(1.0f, 1.0f, 1.0f)),
                                                         DataLine(0.9f))));


    //Final render.
    finalScreenMatChannels[RC::RC_VertexPosOutput] = DataNodeGenerators::ObjectPosToScreenPos<DrawingQuad>(0);
    finalScreenMatChannels[RC::RC_VERTEX_OUT_1] = DataLine(DNP(new VertexInputNode(DrawingQuad::GetAttributeData())), 1);
    DNP finalTexSampler(new TextureSampleNode(DataLine(DNP(new FragmentInputNode(DrawingQuad::GetAttributeData())), 1), "u_finalRenderSample"));
    finalScreenMatChannels[RC::RC_Color] = DataLine(finalTexSampler, TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
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
    water->Params.TextureUniforms[texSamplerName] =
        UniformSamplerValue(&waterNormalTex, texSamplerName,
                            waterMat->GetUniforms(RenderPasses::BaseComponents).FindUniform(texSamplerName, waterMat->GetUniforms(RenderPasses::BaseComponents).TextureUniforms).Loc);

    water->AddFlow(Water::DirectionalWaterArgs(Vector2f(2.0f, 0.0f), 10.0f, 50.0f));


    //Post-process chain.
    ppc = new PostProcessChain(ppcChain, windowSize.x, windowSize.y, manager);
    if (ppc->HasError())
    {
        std::cout << "Error creating post-process chain: " << ppc->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
}


OpenGLTestWorld::OpenGLTestWorld(void)
: SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(24, 0, 0, 3, 3)),
  water(0), ppc(0), finalScreenQuad(0), finalScreenMat(0),
  gsTestMat(0), gsMesh(PrimitiveTypes::Points),
  particleMat(0), particleMesh(PrimitiveTypes::Points),
  particleManager(Textures, particleParams)
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
    cam.Info.zFar = 900.0f;
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
}
void OpenGLTestWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(water);
    DeleteAndSetToNull(waterMat);
    DeleteAndSetToNull(ppc);
    DeleteAndSetToNull(finalScreenQuad);
    DeleteAndSetToNull(finalScreenMat);
    DeleteAndSetToNull(particleMat);
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
        water->AddRipple(Water::RippleWaterArgs(cam.GetPosition(), 5000.0f, 10.0f, 120.0f, 1.0f));
}

void OpenGLTestWorld::RenderWorldGeometry(const RenderInfo & info)
{
    //TODO: Refactor this so it ACTUALLY only renders world geometry.

    //Render the world into a render target.

    manager[worldRenderID]->EnableDrawingInto();
    ScreenClearer().ClearScreen();

    std::vector<const Mesh*> meshList;

    meshList.insert(meshList.end(), &water->GetMesh());
    if (!waterMat->Render(RenderPasses::BaseComponents, info, meshList, water->Params))
    {
        std::cout << "Error rendering water: " << waterMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    meshList.clear();
    meshList.insert(meshList.end(), &gsMesh);
    if (!gsTestMat->Render(RenderPasses::BaseComponents, info, meshList, gsTestParams))
    {
        std::cout << "Error rendering geometry shader test: " << gsTestMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    meshList.clear();
    meshList.insert(meshList.end(), &particleMesh);
    if (!particleMat->Render(RenderPasses::BaseComponents, info, meshList, particleParams))
    {
        std::cout << "Error rendering particles: " << particleMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    manager[worldRenderID]->DisableDrawingInto(windowSize.x, windowSize.y);

    std::string err = GetCurrentRenderingError();
    if (!err.empty())
    {
        std::cout << "Error rendering world geometry: " << err << "\n";
        Pause();
        EndWorld();
        return;
    }

    //Render post-process effects on top of the world.
    if (!ppc->RenderChain(this, cam.Info, manager[worldRenderID]->GetColorTextures()[0], manager[worldRenderID]->GetDepthTexture()))
    {
        std::cout << "Error rendering post-process chain: " << ppc->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
    
    ScreenClearer().ClearScreen();
    //Render the final image.
    Camera cam;
    TransformObject trans;
    Matrix4f identity;
    identity.SetAsIdentity();
    RenderTarget * finalRend = ppc->GetFinalRender();
    if (finalRend == 0) finalRend = manager[worldRenderID];
    finalScreenQuadParams.TextureUniforms["u_finalRenderSample"].Texture.SetData(finalRend->GetColorTextures()[0]);
    if (!finalScreenQuad->Render(RenderPasses::BaseComponents, RenderInfo(this, &cam, &trans, &identity, &identity, &identity), finalScreenQuadParams, *finalScreenMat))
    {
        std::cout << "Error rendering final screen output: " << finalScreenMat->GetErrorMsg() << "\n";
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

	RenderInfo info((SFMLOpenGLWorld*)this, (Camera*)&cam, &dummy, &worldM, &viewM, &projM);

    //Draw the world.
	ScreenClearer().ClearScreen();
    worldRenderState.EnableState();
	RenderWorldGeometry(info);
}


void OpenGLTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
	ClearAllRenderingErrors();

	glViewport(0, 0, newW, newH);
	cam.Info.Width = (float)newW;
	cam.Info.Height = (float)newH;
    windowSize.x = newW;
    windowSize.y = newH;
    if (!manager.ResizeTarget(worldRenderID, newW, newH))
    {
        std::cout << "Error resizing world render target: " << manager.GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
    if (!ppc->ResizeRenderTargets(newW, newH))
    {
        std::cout << "Error resizing PPC render target: " << manager.GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
}