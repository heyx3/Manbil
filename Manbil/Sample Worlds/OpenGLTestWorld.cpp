#include "OpenGLTestWorld.h"


#include <iostream>

#include "../Rendering/Basic Rendering/Material.h"
#include "../Rendering/Basic Rendering/ScreenClearer.h"
#include "../Rendering/Basic Rendering/RenderingState.h"
#include "../Input/Input Objects/MouseBoolInput.h"
#include "../Math/Higher Math/BumpmapToNormalmap.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/Data Nodes/ShaderGenerator.h"
#include "../Math/NoiseGeneration.hpp"
#include "../Rendering/GPU Particles/GPUParticleGenerator.h"
#include "../Rendering/GPU Particles/GPUParticleNodes.h"
#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/GUI/TextRenderer.h"
#include "../Rendering/Primitives/PrimitiveGenerator.h"

#include <assert.h>


typedef MaterialConstants MC;


namespace OGLTestPrints
{
	bool PrintRenderError(const char* errorIntro)
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
const RenderingState worldRenderState(RenderingState::C_NONE);
std::string texSamplerName = "",
            texSampler2Name = "",
            cubeSamplerName = "";
const unsigned int maxRipples = 3,
                   maxFlows = 2;


void OpenGLTestWorld::InitializeTextures(void)
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    //World render target.

    worldColorTex1.Create();
    worldColorTex1.ClearData(windowSize.x, windowSize.y);
    worldColorTex2.Create();
    worldColorTex2.ClearData(windowSize.x, windowSize.y);
    worldDepthTex.Create();

    std::string errorMsg;
    worldRenderID = RenderTargets->CreateRenderTarget(PixelSizes::PS_16U_DEPTH, errorMsg);
    if (worldRenderID == RenderTargetManager::ERROR_ID)
    {
        std::cout << "Error creating world render target: " << errorMsg << "\n";
        Pause();
        EndWorld();
        return;
    }

    std::vector<RenderTargetTex> colorTargTexes;
    colorTargTexes.push_back(RenderTargetTex(&worldColorTex1));
    colorTargTexes.push_back(RenderTargetTex(&worldColorTex2));
    if (!(*RenderTargets)[worldRenderID]->SetDepthAttachment(RenderTargetTex(&worldDepthTex)))
    {
        std::cout << "Error attaching depth texture for world render target.\n";
        Pause();
        EndWorld();
        return;
    }
    if (!(*RenderTargets)[worldRenderID]->SetColorAttachments(colorTargTexes, true))
    {
        std::cout << "Error attaching color textures for world render target\n";
        Pause();
        EndWorld();
        return;
    }


    //Water normal-map 1 creation.

    waterNormalTex1.Create();
    std::string error;
    if (!waterNormalTex1.SetDataFromFile("Content/Textures/Normalmap.png", error))
    {
        std::cout << "Failed to load 'Content/Textures/Normalmap.png': " + error + "\n";
        Pause();
        EndWorld();
        return;
    }


    //Water normal-map 2 creation.

    Perlin2D pers[4] =
    {
        Perlin2D(Vector2f(128, 128), Perlin2D::Quintic, Vector2i(), 12512, true, Vector2u(64, 64)),
        Perlin2D(Vector2f(64, 64), Perlin2D::Quintic, Vector2i(), 1166223, true, Vector2u(32, 32)),
        Perlin2D(Vector2f(32, 32), Perlin2D::Quintic, Vector2i(), 676232, true, Vector2u(16, 16)),
        Perlin2D(Vector2f(16, 16), Perlin2D::Quintic, Vector2i(), 39863984, true, Vector2u(8, 8)),
    };
    Generator2D*const perPointers[4] = { &pers[0], &pers[1], &pers[2], &pers[3] };
    float weights[4] = { 0.5f, 0.25f, 0.125f, 0.06125f };
    LayeredOctave2D layeredPers(4, weights, perPointers);
    
    Array2D<float> noise(512, 512, 0.0f);
    layeredPers.Generate(noise);

    Array2D<Vector3f> bumpNormals(512, 512);
    BumpmapToNormalmap::Convert(noise, 100.0f, true, bumpNormals);

    Array2D<Vector4f> finalNormals(512, 512);
    finalNormals.FillFunc([&bumpNormals](Vector2u loc, Vector4f* outV)
    {
        *outV = Vector4f(bumpNormals[loc], 1.0f);
    });

    waterNormalTex2.Create();
    if (!waterNormalTex2.SetColorData(finalNormals))
    {
        std::cout << "Failed to set generated normalmap data for second normal map texture.\n";
        Pause();
        EndWorld();
        return;
    }


    //Cubemap creation.
    cubemapTex.Create();
    if (false)
    {
        //Wall textures.
        Array2D<Vector4f> cubemapNegX(2, 2), cubemapNegY(2, 2),
                          cubemapPosX(2, 2), cubemapPosY(2, 2);
        cubemapNegX.FillFunc([](Vector2u loc, Vector4f * outVal)
                             {
                                 *outVal = Vector4f(0.25f, 0.25f, (float)loc.y, 1.0f);
                             });
        cubemapNegY.FillFunc([](Vector2u loc, Vector4f * outVal)
                             {
                                 *outVal = Vector4f(0.25f, 0.25f, 1.0f - (float)loc.y, 1.0f);
                             });
        cubemapPosX.FillFunc([](Vector2u loc, Vector4f * outVal)
                             {
                                 *outVal = Vector4f(0.25f, 0.25f, (float)loc.y, 1.0f);
                             });
        cubemapPosY.FillFunc([](Vector2u loc, Vector4f * outVal)
                             {
                                 *outVal = Vector4f(0.25f, 0.25f, (float)loc.y, 1.0f);
                             });
        //Rotate faces until they're facing the right way.
        Array2D<Vector4f> tempArr(cubemapPosX.GetWidth(), cubemapPosX.GetHeight());
        tempArr.Fill(cubemapPosX.GetArray(), true);
        tempArr.RotateInto(3, cubemapPosX, true);
        tempArr.Fill(cubemapNegX.GetArray(), true);
        tempArr.RotateInto(1, cubemapNegX, true);
        tempArr.Reset(1, 1);

        //Floor/ceiling textures.
        Array2D<Vector4f> cubemapFloor(2, 2, Vector4f(0.25f, 0.25f, 0.0f, 1.0f)),
                          cubemapCeiling(2, 2, Vector4f(0.25f, 0.25f, 1.0f, 0.0f));

        cubemapTex.SetDataColor(cubemapNegX, cubemapNegY, cubemapFloor,
                                cubemapPosX, cubemapPosY, cubemapCeiling);
    }
    else
    {
        error = cubemapTex.SetDataFromFiles("Content/Cubemaps/sky_neg_x.png",
                                            "Content/Cubemaps/sky_neg_y.png",
                                            "Content/Cubemaps/sky_pos_z.png",
                                            "Content/Cubemaps/sky_pos_x.png",
                                            "Content/Cubemaps/sky_pos_y.png",
                                            "Content/Cubemaps/sky_neg_z.png");
        if (!error.empty())
        {
            std::cout << "Error loading cubemap textures: " << error << "\n";
            Pause();
            EndWorld();
            return;
        }
    }
}
void OpenGLTestWorld::InitializeMaterials(void)
{
    typedef DataNode::Ptr DNP;

    std::vector<ShaderOutput> &vertOuts = DataNode::MaterialOuts.VertexOutputs,
                              &fragOuts = DataNode::MaterialOuts.FragmentOutputs;

    {
        #pragma region Water


        //Vertex output 0: object-space position.
        //Vertex output 1: UV coords.
        //Vertex output 2: world-space position.


        DataNode::ClearMaterialData();
        DataNode::VertexIns = WaterVertex::GetAttributeData();

        //Vertex position output.
        DNP waterCalcs(new WaterNode(DataLine(VertexInputNode::GetInstance()),
                                     DataLine(FragmentInputNode::GetInstance()),
                                     3, 2, "waterCalculations"));
        DNP screenPos(new SpaceConverterNode(DataLine(waterCalcs, WaterNode::GetVertexPosOutputIndex()),
                                             SpaceConverterNode::ST_OBJECT,
                                             SpaceConverterNode::ST_SCREEN,
                                             SpaceConverterNode::DT_POSITION,
                                             "objToScreenPos"));
        DataNode::MaterialOuts.VertexPosOutput = DataLine(screenPos, 1);

        //Vertex shader outputs.
        DNP worldPos(new SpaceConverterNode(DataLine(waterCalcs, WaterNode::GetVertexPosOutputIndex()),
                                            SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_WORLD,
                                            SpaceConverterNode::DT_POSITION, "objToWorldPos"));
        vertOuts.push_back(ShaderOutput("vOut_objPos",
                                        DataLine(waterCalcs, WaterNode::GetVertexPosOutputIndex())));
        vertOuts.push_back(ShaderOutput("vOut_UV", DataLine(VertexInputNode::GetInstance(), 1)));
        vertOuts.push_back(ShaderOutput("vOut_worldPos", DataLine(worldPos)));

        //Fragment shader outputs.
        DataLine normalMap1Scale(10.0f),
                 normalMap1Pan(Vector2f(-0.09f, 0.01f));
        DNP normalMap1Scaled(new MultiplyNode(DataLine(FragmentInputNode::GetInstance(), 1),
                                              normalMap1Scale,
                                              "normalMap1Scaled")),
            normalMap1PanAmount(new MultiplyNode(normalMap1Pan, TimeNode::GetInstance(),
                                                 "normalMap1PanAmount")),
            normalMap1Panned(new AddNode(normalMap1Scaled, normalMap1PanAmount, "normalMap1Panned"));
        DNP normalMap1Ptr(new TextureSample2DNode(normalMap1Panned, "u_normalMap1Tex",
                                                  "normalMapSample1"));
        DataLine normalMap1(normalMap1Ptr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
        DNP finalNormalMap1(new RemapNode(normalMap1,
                                          Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f),
                                          Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(1.0f, 1.0f, 1.0f),
                                          "finalNormalMap1"));

        DataLine normalMap2Scale(3.0f),
                 normalMap2Pan(Vector2f(0.015f, 0.05f));
        DNP normalMap2Scaled(new MultiplyNode(DataLine(FragmentInputNode::GetInstance(), 1),
                                              normalMap2Scale,
                                              "normalMapScaled2")),
            normalMap2PanAmount(new MultiplyNode(normalMap2Pan, TimeNode::GetInstance())),
            normalMap2Panned(new AddNode(normalMap2Scaled, normalMap2PanAmount));
        DNP normalMap2Ptr(new TextureSample2DNode(normalMap2Panned, "u_normalMap2Tex",
                                                  "normalMapSample2"));
        DataLine normalMap2(normalMap2Ptr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
        DNP finalNormalMap2(new RemapNode(normalMap2,
                                          Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f),
                                          Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(1.0f, 1.0f, 1.0f),
                                          "finalNormalMap2"));

        DNP combineNormalMaps(new TangentSpaceNormalsNode(finalNormalMap1, finalNormalMap2,
                                                          "combinedNormals"));

        DNP applyNormalMap(new AddNode(combineNormalMaps,
                                       DataLine(waterCalcs,
                                                WaterNode::GetSurfaceNormalOutputIndex()),
                                       "afterNormalMapping"));
        DNP finalObjNormal(new NormalizeNode(applyNormalMap, "finalObjNormal"));
        DNP worldNormal(new SpaceConverterNode(finalObjNormal,
                                               SpaceConverterNode::ST_OBJECT,
                                               SpaceConverterNode::ST_WORLD,
                                               SpaceConverterNode::DT_NORMAL,
                                               "worldNormal"));
        DNP finalWorldNormal(new NormalizeNode(worldNormal, "finalWorldNormal"));

        DNP eyeRay(new SubtractNode(worldPos, CameraDataNode::GetCamPos(), "eyeRay")),
            reflectedEyeRay(new ReflectNode(eyeRay, finalWorldNormal, "reflectedEyeRay"));
        DNP cubemapSamplePtr(new TextureSampleCubemapNode(reflectedEyeRay, "u_cubemapTex",
                                                          "cubemapSample"));
        DataLine cubemapSampleRGB(cubemapSamplePtr,
                                  TextureSampleCubemapNode::GetOutputIndex(CO_AllColorChannels));

        DNP lightCalc(new LightingNode(DataLine(FragmentInputNode::GetInstance(), 2),
                                       finalWorldNormal,
                                       DataLine(Vector3f(0.6f, -1.0f, -0.1f).Normalized()),
                                       "lightCalc",
                                       DataLine(0.3f), DataLine(0.7f),
                                       DataLine(3.0f), DataLine(64.0f)));
        DNP litColorRGB(new MultiplyNode(lightCalc, DataLine(Vector3f(0.275f, 0.275f, 1.0f)),
                                         "litColorRGB"));

        DNP finalColorRGB(new InterpolateNode(litColorRGB, cubemapSampleRGB, 0.65f,
                                              InterpolateNode::IT_Linear, "lerpCubemap")),
            finalColor(new CombineVectorNode(finalColorRGB, 1.0f, "finalColor"));

        DNP worldNormalToTexValue(new RemapNode(finalWorldNormal,
                                                DataLine(-1.0f), DataLine(1.0f),
                                                DataLine(0.0f), DataLine(1.0f),
                                                "worldNormToTexVal"));
        DNP worldNormalColor(new CombineVectorNode(worldNormalToTexValue, 1.0f, "worldNormalColor"));

        fragOuts.push_back(ShaderOutput("fOut_FinalColor", finalColor));
        fragOuts.push_back(ShaderOutput("fOut_WorldNormal", worldNormalColor));

        texSamplerName = ((TextureSample2DNode*)normalMap1Ptr.get())->SamplerName;
        texSampler2Name = ((TextureSample2DNode*)normalMap2Ptr.get())->SamplerName;
        cubeSamplerName = ((TextureSampleCubemapNode*)cubemapSamplePtr.get())->SamplerName;

        UniformDictionary unDict;
        ShaderGenerator::GeneratedMaterial wM =
            ShaderGenerator::GenerateMaterial(unDict, BlendMode::GetOpaque());
        if (!wM.ErrorMessage.empty())
        {
            std::cout << "Error generating water material: " << wM.ErrorMessage << "\n";
            Pause();
            EndWorld();
            return;
        }
        waterMat = wM.Mat;


        #pragma endregion
    }

    {
        #pragma region Particles


        particleMesh.SubMeshes.push_back(MeshData(false, PT_POINTS));
        MeshData& partMeshDat = particleMesh.SubMeshes[0];
        GPUParticleGenerator::GenerateGPUPParticles(partMeshDat, GPUParticleGenerator::NOP_16384);

        //Position.
        std::unordered_map<GPUPOutputs, DataLine> gpupOuts;
        DataNode::Ptr randVals = GetRandSeeds(0, 1, 2);
        DataNode::Ptr randValSplit(new VectorComponentsNode(randVals, "randValComponents"));
        DataNode::Ptr normX(new RemapNode(DataLine(randValSplit, 0), 0.0f, 1.0f, -1.0f, 1.0f, "normX")),
                      normY(new RemapNode(DataLine(randValSplit, 1), 0.0f, 1.0f, -1.0f, 1.0f, "normY")),
                      normZ(new RemapNode(DataLine(randValSplit, 2), 0.0f, 1.0f, -1.0f, 1.0f, "normZ"));
        DataLine randRadiusLerp = GetRandSeedFloat(3);
        DataNode::Ptr randRadius(new MultiplyNode(randRadiusLerp, 100.0f, "randRadius"));
        DataNode::Ptr partPosition(new PosInSphereNode(normX, normY, normZ,
                                                       Vector3f(500.0f, 500.0f, 100.0f), randRadius,
                                                       "spherePos"));
        gpupOuts[GPUP_WORLDPOSITION] = partPosition;

        //Rotation.
        DataNode::Ptr rotSpeed(new InterpolateNode(-5.0f, 5.0f, GetRandSeedFloat(3),
                                                   InterpolateNode::IT_Linear, "rotSpeed"));
        DataNode::Ptr partRotation(new MultiplyNode(TimeNode::GetInstance(), rotSpeed, "rotation"));
        gpupOuts[GPUP_QUADROTATION] = partRotation;

        //Color.
        DataNode::Ptr redGreen = GetRandSeeds(4, 5);
        DataNode::Ptr finalColor(new CombineVectorNode(redGreen, 0.0f, 1.0f, "finalColorNode"));
        gpupOuts[GPUP_COLOR] = finalColor;

        ShaderGenerator::GeneratedMaterial partM =
            GPUParticleGenerator::GenerateMaterial(gpupOuts, particleParams,
                                                   BlendMode::GetTransparent());
        if (!partM.ErrorMessage.empty())
        {
            std::cout << "Error generating material for particle effect: " << partM.ErrorMessage << "\n";
            Pause();
            EndWorld();
            return;
        }
        particleMat = partM.Mat;


        #pragma endregion
    }

    {
        #pragma region Cubemap


        DataNode::ClearMaterialData();
        DataNode::VertexIns = PrimitiveGenerator::CubemapVertex::GetAttributeData();

        //Vertex pos output.
        DataLine worldPos = VertexInputNode::GetInstance();
        DataLine worldScale = 2800.0f;
        DNP scaledWorldPos(new MultiplyNode(worldPos, worldScale, "scaleWorldPos"));
        DNP centeredWorldPos(new AddNode(scaledWorldPos, CameraDataNode::GetCamPos(),
                                         "centerWorldPos"));
        DNP screenPos(new SpaceConverterNode(centeredWorldPos,
                                             SpaceConverterNode::ST_WORLD, SpaceConverterNode::ST_SCREEN,
                                             SpaceConverterNode::DT_POSITION,
                                             "worldPosToScreenPos"));
        DataNode::MaterialOuts.VertexPosOutput = DataLine(screenPos, 1);

        //Vertex shader outputs.
        vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_worldPos", worldPos));

        //Fragment shader outputs.
        DataLine cubemapUVs = FragmentInputNode::GetInstance();
        DNP cubemapSamplePtr(new TextureSampleCubemapNode(cubemapUVs, "u_cubemapTex",
                                                          "cubemapSample"));
        DataLine cubemapSampleRGB(cubemapSamplePtr,
                                  TextureSampleCubemapNode::GetOutputIndex(CO_AllColorChannels));
        DNP finalCubeColor(new CombineVectorNode(cubemapSampleRGB, 1.0f, "finalCubeColor"));
        fragOuts.insert(fragOuts.end(), ShaderOutput("vOut_FinalColor", finalCubeColor));

        ShaderGenerator::GeneratedMaterial cmGen =
            ShaderGenerator::GenerateMaterial(cubemapParams, BlendMode::GetOpaque());
        if (!cmGen.ErrorMessage.empty())
        {
            std::cout << "Error generating shaders for cubemap material: " << cmGen.ErrorMessage << "\n";
            Pause();
            EndWorld();
            return;
        }
        cubemapParams.TextureCubemaps["u_cubemapTex"].Texture = cubemapTex.GetTextureHandle();
        cubemapMat = cmGen.Mat;


        #pragma endregion
    }

    {
        #pragma region Post-process and final render


        //Final render.

        DataNode::ClearMaterialData();
        DataNode::VertexIns = DrawingQuad::GetVertexInputData();

        DNP objToScreenPos(new SpaceConverterNode(VertexInputNode::GetInstance(),
                                                  SpaceConverterNode::ST_OBJECT,
                                                  SpaceConverterNode::ST_WORLD,
                                                  SpaceConverterNode::DT_POSITION,
                                                  "objToScreenPos"));
        DataNode::MaterialOuts.VertexPosOutput = DataLine(objToScreenPos, 1);

        vertOuts.push_back(ShaderOutput("vOut_UV", DataLine(VertexInputNode::GetInstance(), 1)));

        DNP texSamplerPtr(new TextureSample2DNode(FragmentInputNode::GetInstance(),
                                                  "u_finalRenderSample", "sampleRender"));
        DataLine texSampler(texSamplerPtr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
        DNP finalRenderCol(new CombineVectorNode(texSampler, 1.0f, "finalRenderCol"));
        fragOuts.push_back(ShaderOutput("fOut_FinalColor", finalRenderCol));

        UniformDictionary uniformDict;
        ShaderGenerator::GeneratedMaterial genM =
            ShaderGenerator::GenerateMaterial(uniformDict, BlendMode::GetOpaque());
        if (!genM.ErrorMessage.empty())
        {
            std::cout << "Error generating shaders for final screen material: " <<
                         genM.ErrorMessage << "\n";
            Pause();
            EndWorld();
            return;
        }
        finalScreenMat = genM.Mat;
        finalScreenQuadParams = uniformDict;


        #pragma endregion
    }
}
void OpenGLTestWorld::InitializeObjects(void)
{
    //Water.

    const unsigned int size = 300;

    water = new Water(size, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(6.0f, 6.0f, 2.0f),
                      maxRipples, maxFlows);
    water->GetTransform().IncrementPosition(Vector3f(0.0f, 0.0f, -10.0f));

    //Set up water parameters.
    water->SetMaterial(waterMat);
    water->Params.Texture2Ds[texSamplerName] =
        UniformValueSampler2D(waterNormalTex1.GetTextureHandle(), texSamplerName,
                              waterMat->GetUniforms().FindUniform(texSamplerName).Loc);
    water->Params.Texture2Ds[texSampler2Name] =
        UniformValueSampler2D(waterNormalTex2.GetTextureHandle(), texSampler2Name,
                              waterMat->GetUniforms().FindUniform(texSampler2Name).Loc);
    water->Params.TextureCubemaps[cubeSamplerName] =
        UniformValueSamplerCubemap(cubemapTex.GetTextureHandle(), cubeSamplerName,
                                   waterMat->GetUniforms().FindUniform(cubeSamplerName).Loc);

    water->AddFlow(Water::DirectionalWaterArgs(Vector2f(2.0f, 0.0f), 10.0f, 50.0f));


    //Cubemap.

    std::vector<PrimitiveGenerator::CubemapVertex> cmVertices;
    std::vector<unsigned int> cmIndices;
    PrimitiveGenerator::GenerateCubemapCube(cmVertices, cmIndices, false, true);
    cubemapMesh.SubMeshes.push_back(MeshData(false, PT_TRIANGLE_LIST));
    MeshData& dat = cubemapMesh.SubMeshes[0];
    dat.SetVertexData(cmVertices, MeshData::BUF_STATIC,
                      PrimitiveGenerator::CubemapVertex::GetAttributeData());
    dat.SetIndexData(cmIndices, MeshData::BUF_STATIC);
}


OpenGLTestWorld::OpenGLTestWorld(void)
    : SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(24, 0, 0, 4, 1)),
      water(0), waterMat(0), finalScreenMat(0), particleMat(0), cubemapMat(0),
      waterNormalTex1(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PS_32F, true),
      waterNormalTex2(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PS_32F, true),
      cubemapTex(TextureSampleSettings3D(FT_LINEAR, WT_CLAMP), PS_32F, true),
      worldColorTex1(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F, false),
      worldColorTex2(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F, false),
      worldDepthTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F_DEPTH, false)
{
}
void OpenGLTestWorld::InitializeWorld(void)
{
	SFMLOpenGLWorld::InitializeWorld();

	if (IsGameOver())
    {
        return;
    }

    DrawingQuad::InitializeQuad();
	
    Input.AddBoolInput(666, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Left,
                                                                        BoolInput::JustPressed)));

	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

    RenderTargets = new RenderTargetManager();

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();


    Vector3f pos(2.0f, 2.0f, 10.0f);
    cam.SetPosition(pos);
    cam.SetRotation(-pos.Normalized(), Vector3f(0.0f, 0.0f, 1.0f));
    cam.Window = GetWindow();
    cam.PerspectiveInfo.SetFOVDegrees(55.0f);
    cam.PerspectiveInfo.zFar = 5000.0f;
    cam.PerspectiveInfo.zNear = 1.0f;
    cam.PerspectiveInfo.Width = windowSize.x;
    cam.PerspectiveInfo.Height = windowSize.y;
    cam.SetMoveSpeed(100.0f);
    cam.SetRotSpeed(0.25f);
}

void OpenGLTestWorld::OnWorldEnd(void)
{
    delete water;
    delete waterMat;
    delete particleMat;
    delete cubemapMat;
    delete finalScreenMat;

    delete RenderTargets;

    waterNormalTex1.DeleteIfValid();
    waterNormalTex2.DeleteIfValid();
    cubemapTex.DeleteIfValid();

    DrawingQuad::DestroyQuad();
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

    if (Input.GetBoolInputValue(666))
    {
        water->AddRipple(Water::RippleWaterArgs(cam.GetPosition(), 5000.0f, 40.0f, 120.0f, 4.0f));
    }
}

void OpenGLTestWorld::RenderWorldGeometry(const RenderInfo& info)
{
    ScreenClearer().ClearScreen();
    worldRenderState.EnableState();

    waterMat->GetBlendMode().EnableMode();
    waterMat->Render(info, &water->MyMesh, water->Params);

    particleMat->GetBlendMode().EnableMode();
    particleMat->Render(info, &particleMesh, particleParams);

    cubemapMat->GetBlendMode().EnableMode();
    cubemapMat->Render(info, &cubemapMesh, cubemapParams);
}

void OpenGLTestWorld::RenderOpenGL(float elapsedSeconds)
{
    //Render the world into a render target.

	Matrix4f viewM, projM;
	cam.GetViewTransform(viewM);
	cam.GetPerspectiveProjection(projM);
    //cam.GetOrthoProjection(projM);

    RenderInfo info(GetTotalElapsedSeconds(), (Camera*)&cam, &viewM, &projM);
    (*RenderTargets)[worldRenderID]->EnableDrawingInto();
    RenderWorldGeometry(info);
    (*RenderTargets)[worldRenderID]->DisableDrawingInto(windowSize.x, windowSize.y, true);


    //Choose which render color target to use as the world render.
    RenderObjHandle worldRendTex;
    bool useSpecial = sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

    if (useSpecial)
    {
        worldRendTex = (*RenderTargets)[worldRenderID]->GetColorTextures()[1].MTex->GetTextureHandle();
    }
    else
    {
        worldRendTex = (*RenderTargets)[worldRenderID]->GetColorTextures()[0].MTex->GetTextureHandle();
    }


    //Render the final image.

    ScreenClearer().ClearScreen();
    Camera cam;
    RenderObjHandle finalRendTex;
    if (useSpecial)
    {
        finalRendTex = (*RenderTargets)[worldRenderID]->GetColorTextures()[1].MTex->GetTextureHandle();
    }
    else
    {
        finalRendTex = (*RenderTargets)[worldRenderID]->GetColorTextures()[0].MTex->GetTextureHandle();
    }
    finalScreenQuadParams.Texture2Ds["u_finalRenderSample"].Texture = finalRendTex;

    Matrix4f identity;
    DrawingQuad::GetInstance()->Render(RenderInfo(GetTotalElapsedSeconds(), &cam, &identity, &identity),
                                       finalScreenQuadParams, *finalScreenMat);
}


void OpenGLTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
	ClearAllRenderingErrors();

	glViewport(0, 0, newW, newH);
	cam.PerspectiveInfo.Width = (float)newW;
	cam.PerspectiveInfo.Height = (float)newH;
    windowSize.x = newW;
    windowSize.y = newH;
    worldColorTex1.ClearData(newW, newH);
    worldColorTex2.ClearData(newW, newH);
    if (!(*RenderTargets)[worldRenderID]->UpdateSize())
    {
        std::cout << "Error resizing world render target.\n";
        Pause();
        EndWorld();
        return;
    }
}