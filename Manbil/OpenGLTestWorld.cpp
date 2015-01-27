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
        error = cubemapTex.SetDataFromFiles("Content/Cubemaps/sky_neg_x.png", "Content/Cubemaps/sky_neg_y.png", "Content/Cubemaps/sky_pos_z.png",
                                            "Content/Cubemaps/sky_pos_x.png", "Content/Cubemaps/sky_pos_y.png", "Content/Cubemaps/sky_neg_z.png");
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
    typedef DataNode::Ptr DNP;

    std::vector<ShaderOutput> & vertOuts = DataNode::MaterialOuts.VertexOutputs,
                              & fragOuts = DataNode::MaterialOuts.FragmentOutputs;

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
                                     "waterCalculations", 3, 2));
        DNP screenPos(new SpaceConverterNode(DataLine(waterCalcs, WaterNode::GetVertexPosOutputIndex()),
                                             SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                             SpaceConverterNode::DT_POSITION, "objToScreenPos"));
        DataNode::MaterialOuts.VertexPosOutput = DataLine(screenPos, 1);

        //Vertex shader outputs.
        DNP worldPos(new SpaceConverterNode(DataLine(waterCalcs, WaterNode::GetVertexPosOutputIndex()),
                                            SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_WORLD,
                                            SpaceConverterNode::DT_POSITION, "objToWorldPos"));
        vertOuts.insert(vertOuts.end(),
                        ShaderOutput("vOut_objPos", DataLine(waterCalcs, WaterNode::GetVertexPosOutputIndex())));
        vertOuts.insert(vertOuts.end(),
                        ShaderOutput("vOut_UV", DataLine(VertexInputNode::GetInstance(), 1)));
        vertOuts.insert(vertOuts.end(),
                        ShaderOutput("vOut_worldPos", DataLine(worldPos)));

        //Fragment shader outputs.
        DataLine normalMap1Scale(10.0f),
                 normalMap1Pan(VectorF(-0.09f, 0.01f));
        DNP normalMap1Scaled(new MultiplyNode(DataLine(FragmentInputNode::GetInstance(), 1), normalMap1Scale, "normalMapScaled1")),
            normalMap1PanAmount(new MultiplyNode(normalMap1Pan, TimeNode::GetInstance())),
            normalMap1Panned(new AddNode(normalMap1Scaled, normalMap1PanAmount));
        DNP normalMap1Ptr(new TextureSample2DNode(normalMap1Panned, "u_normalMap1Tex", "normalMapSample1"));
        DataLine normalMap1(normalMap1Ptr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
        DNP finalNormalMap1(new RemapNode(normalMap1,
                                          Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f),
                                          Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(1.0f, 1.0f, 1.0f)));

        DataLine normalMap2Scale(3.0f),
                 normalMap2Pan(VectorF(0.015f, 0.05f));
        DNP normalMap2Scaled(new MultiplyNode(DataLine(FragmentInputNode::GetInstance(), 1), normalMap2Scale, "normalMapScaled2")),
            normalMap2PanAmount(new MultiplyNode(normalMap2Pan, TimeNode::GetInstance())),
            normalMap2Panned(new AddNode(normalMap2Scaled, normalMap2PanAmount));
        DNP normalMap2Ptr(new TextureSample2DNode(normalMap2Panned, "u_normalMap2Tex", "normalMapSample2"));
        DataLine normalMap2(normalMap2Ptr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
        DNP finalNormalMap2(new RemapNode(normalMap2,
                                          Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 1.0f, 1.0f),
                                          Vector3f(-1.0f, -1.0f, -1.0f), Vector3f(1.0f, 1.0f, 1.0f)));

        DNP combineNormalMaps(new TangentSpaceNormalsNode(finalNormalMap1, finalNormalMap2, "combinedNormals"));

        DNP applyNormalMap(new AddNode(combineNormalMaps, DataLine(waterCalcs, WaterNode::GetSurfaceNormalOutputIndex()), "afterNormalMapping"));
        DNP finalObjNormal(new NormalizeNode(applyNormalMap, "finalObjNormal"));
        DNP worldNormal(new SpaceConverterNode(finalObjNormal, SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_WORLD,
                                               SpaceConverterNode::DT_NORMAL, "worldNormal"));
        DNP finalWorldNormal(new NormalizeNode(worldNormal));

        DNP eyeRay(new SubtractNode(worldPos, CameraDataNode::GetCamPos(), "eyeRay")),
            reflectedEyeRay(new ReflectNode(eyeRay, finalWorldNormal, "reflectedEyeRay"));
        DNP cubemapSamplePtr(new TextureSampleCubemapNode(reflectedEyeRay, "u_cubemapTex", "cubemapSample"));
        DataLine cubemapSampleRGB(cubemapSamplePtr, TextureSampleCubemapNode::GetOutputIndex(CO_AllColorChannels));

        DNP lightCalc(new LightingNode(DataLine(FragmentInputNode::GetInstance(), 2),
                                       finalWorldNormal, DataLine(Vector3f(0.6f, -1.0f, -0.1f).Normalized()),
                                       "lightCalc", DataLine(0.3f), DataLine(0.7f), DataLine(3.0f), DataLine(64.0f)));
        DNP litColorRGB(new MultiplyNode(lightCalc, DataLine(VectorF(0.275f, 0.275f, 1.0f)), "litColorRGB"));

        DNP finalColorRGB(new InterpolateNode(litColorRGB, cubemapSampleRGB, 0.65f, InterpolateNode::IT_Linear, "lerpCubemap")),
            finalColor(new CombineVectorNode(finalColorRGB, 1.0f, "finalColor"));

        DNP worldNormalToTexValue(new RemapNode(finalWorldNormal, DataLine(-1.0f), DataLine(1.0f), DataLine(0.0f), DataLine(1.0f), "worldNormToTexVal"));
        DNP worldNormalColor(new CombineVectorNode(worldNormalToTexValue, 1.0f, "worldNormalColor"));

        fragOuts.insert(fragOuts.end(), ShaderOutput("fOut_FinalColor", finalColor));
        fragOuts.insert(fragOuts.end(), ShaderOutput("fOut_WorldNormal", worldNormalColor));

        texSamplerName = ((TextureSample2DNode*)normalMap1Ptr.get())->SamplerName;
        texSampler2Name = ((TextureSample2DNode*)normalMap2Ptr.get())->SamplerName;
        cubeSamplerName = ((TextureSampleCubemapNode*)cubemapSamplePtr.get())->SamplerName;

        UniformDictionary unDict;
        ShaderGenerator::GeneratedMaterial wM = ShaderGenerator::GenerateMaterial(unDict, RenderingModes::RM_Opaque);
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
    }

    {
        #pragma region Geometry shader test

        
        DataNode::ClearMaterialData();
        DataNode::VertexIns = VertexPos::GetAttributeData();

        //Vertex pos output.
        DNP worldPos(new SpaceConverterNode(DataLine(VertexInputNode::GetInstance()),
                                            SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_WORLD,
                                            SpaceConverterNode::DT_POSITION, "worldPos"));
        DNP vertexPosOut(new CombineVectorNode(worldPos, 1.0f));
        DataNode::MaterialOuts.VertexPosOutput = DataLine(vertexPosOut);

        //Geometry Outputs.
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
        DataNode::GeometryShader = GeoShaderData(ShaderInOutAttributes(2, false, "UVs"), geoShaderUsage,
                                                 4, Points, TriangleStrip, gsTestParams, geoCode);

        //Fragment outputs.
        DNP timeTexUVs2D(new MultiplyNode(0.25f, TimeNode::GetTime(), "texUVs2D")),
            timeTexUVs3D(new CombineVectorNode(FragmentInputNode::GetInstance(), timeTexUVs2D, "texUVs3D")),
            timeTexSample(new TextureSample3DNode(timeTexUVs3D, "u_tex3D", "tex3DSample")),
            timeTexSampleFinal(new CombineVectorNode(DataLine(timeTexSample,
                                                              TextureSample3DNode::GetOutputIndex(CO_AllColorChannels)),
                                                     1.0f, "tex3DSampleFinal"));
        DNP textSamplerPtr(new TextureSample2DNode(FragmentInputNode::GetInstance(), "u_textSampler", "textSampler"));
        DataLine textSamplerRed(textSamplerPtr, TextureSample2DNode::GetOutputIndex(CO_Red));
        DNP textSamplerFinal(new CombineVectorNode(textSamplerRed, textSamplerRed, textSamplerRed, 1.0f, "textSamplerFinal"));
        DNP finalDiffuse(new MultiplyNode(textSamplerFinal, timeTexSampleFinal));
        fragOuts.insert(fragOuts.end(), ShaderOutput("fOut_FinalColor", finalDiffuse));
    
        //Material generation.
        ShaderGenerator::GeneratedMaterial gsGen = ShaderGenerator::GenerateMaterial(gsTestParams, RenderingModes::RM_Opaque);
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
    }

    {
        #pragma region Particles


        std::unordered_map<GPUPOutputs, DataLine> gpupOuts;
        HGPComponentManager manager(particleParams);
        /*
        const unsigned int posSeeds[] = { 5, 1, 3, 2 },
                           velSeeds[] = { 0, 1, 2 },
                           accelSeeds[] = { 3, 4, 5 };
        HGPComponentPtr(3) initialPos(new SpherePositionComponent(manager, Vector3f(500.0f, 500.0f, 50.0f), 20.0f, posSeeds, "initialPos"));
        HGPComponentPtr(3) initialVel(new RandomizedHGPComponent<3>(manager, HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(80.0f, 80.0f, 30.0f), manager, "minVel")),
                                                                    HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(-80.0f, -80.0f, 70.0f), manager, "maxVel")),
                                                                    velSeeds, "initialVel")),
                           accel(new RandomizedHGPComponent<3>(manager, HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(0.0f, 0.0f, -30.0f), manager, "minAccel")),
                                                               HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(0.0f, 0.0f, -90.0f), manager, "maxAccel")),
                                                               accelSeeds, "accel"));
        const unsigned int sizeSeeds[] = { 3, 1 },
                           colorSeeds[] = { 0, 4, 2, 5 };
        manager.SetWorldPosition(HGPComponentPtr(3)(new ConstantAccelerationHGPComponent(manager, accel, initialVel, initialPos, "kinematicComp")));
        manager.SetSize(HGPComponentPtr(2)(new RandomizedHGPComponent<2>(manager, HGPComponentPtr(2)(new ConstantHGPComponent<2>(VectorF((unsigned int)2, 0.1f), manager, "minSize")),
                                                                         HGPComponentPtr(2)(new ConstantHGPComponent<2>(VectorF((unsigned int)2, 1.0f), manager, "maxSize")),
                                                                         sizeSeeds, "sizeComp")));
        manager.SetColor(HGPComponentPtr(4)(new RandomizedHGPComponent<4>(manager, HGPComponentPtr(4)(new ConstantHGPComponent<4>(VectorF(0.0f, 0.0f, 0.0f, 1.0f), manager, "minColor")),
                                                                          HGPComponentPtr(4)(new ConstantHGPComponent<4>(VectorF((unsigned int)4, 1.0f), manager, "maxColor")),
                                                                          colorSeeds, "colorComp")));
        */
        manager.SetWorldPosition(HGPComponentPtr(3)(new ConstantHGPComponent<3>(Vector3f(500.0f, 500.0f, 50.0f), manager, "constWorldPos")));
        manager.SetSize(HGPComponentPtr(2)(new ConstantHGPComponent<2>(Vector2f(0.5f, 0.5f), manager, "constSize")));
        manager.SetColor(HGPComponentPtr(4)(new ConstantHGPComponent<4>(Vector4f(0.5f, 0.5f, 0.5f, 1.0f), manager, "constColor")));
        manager.SetGPUPOutputs(gpupOuts);
        manager.Initialize();

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
        particleMesh.SubMeshes.insert(particleMesh.SubMeshes.end(),
                                      VertexIndexData(GPUParticleGenerator::GetNumbParticles(numb),
                                                      GPUParticleGenerator::GenerateGPUPParticles(numb)));


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
        DNP centeredWorldPos(new AddNode(scaledWorldPos, CameraDataNode::GetCamPos(), "centerWorldPos"));
        DNP screenPos(new SpaceConverterNode(centeredWorldPos,
                                             SpaceConverterNode::ST_WORLD, SpaceConverterNode::ST_SCREEN,
                                             SpaceConverterNode::DT_POSITION,
                                             "worldPosToScreenPos"));
        DataNode::MaterialOuts.VertexPosOutput = DataLine(screenPos, 1);

        //Vertex shader outputs.
        vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_worldPos", worldPos));

        //Fragment shader outputs.
        DataLine cubemapUVs = FragmentInputNode::GetInstance();
        DNP cubemapSamplePtr(new TextureSampleCubemapNode(cubemapUVs, "u_cubemapTex", "cubemapSample"));
        DataLine cubemapSampleRGB(cubemapSamplePtr, TextureSampleCubemapNode::GetOutputIndex(CO_AllColorChannels));
        DNP finalCubeColor(new CombineVectorNode(cubemapSampleRGB, 1.0f, "finalCubeColor"));
        fragOuts.insert(fragOuts.end(), ShaderOutput("vOut_FinalColor", finalCubeColor));

        ShaderGenerator::GeneratedMaterial cmGen = ShaderGenerator::GenerateMaterial(cubemapParams, RenderingModes::RM_Opaque);
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
    }

    {
        #pragma region Post-process and final render


        //Post-processing.
        typedef PostProcessEffect::PpePtr PpePtr;
        ppcChain.insert(ppcChain.end(), PpePtr(new FogEffect(DataLine(1.5f), DataLine(Vector3f(1.0f, 1.0f, 1.0f)), DataLine(0.0001f))));


        //Final render.

        DataNode::ClearMaterialData();
        DataNode::VertexIns = DrawingQuad::GetAttributeData();

        DNP objToScreenPos(new SpaceConverterNode(VertexInputNode::GetInstance(),
                                                  SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_WORLD,
                                                  SpaceConverterNode::DT_POSITION, "objToScreenPos"));
        DataNode::MaterialOuts.VertexPosOutput = DataLine(objToScreenPos, 1);

        vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_UV", DataLine(VertexInputNode::GetInstance(), 1)));

        DNP texSamplerPtr(new TextureSample2DNode(FragmentInputNode::GetInstance(),
                                                  "u_finalRenderSample", "sampleRender"));
        DataLine texSampler(texSamplerPtr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
        DNP finalRenderCol(new CombineVectorNode(texSampler, 1.0f, "finalRenderCol"));
        fragOuts.insert(fragOuts.end(), ShaderOutput("fOut_FinalColor", finalRenderCol));

        UniformDictionary uniformDict;
        ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(uniformDict, RenderingModes::RM_Opaque);
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
}
void OpenGLTestWorld::InitializeObjects(void)
{
    //Set up geometry shader mesh.
    RenderObjHandle gsVBO;
    Vector3f vertex[1] = { Vector3f(0.0f, 0.0f, 0.0f) };
    RenderDataHandler::CreateVertexBuffer(gsVBO, &vertex, 1, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    gsMesh.SubMeshes.insert(gsMesh.SubMeshes.end(), VertexIndexData(1, gsVBO));
    gsMesh.Transform.SetPosition(Vector3f(90.0f, 90.0f, 90.0f));


    //Water.

    const unsigned int size = 300;

    water = new Water(size, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(6.0f, 6.0f, 2.0f),
                      OptionalValue<Water::RippleWaterCreationArgs>(Water::RippleWaterCreationArgs(maxRipples)),
                      OptionalValue<Water::DirectionalWaterCreationArgs>(Water::DirectionalWaterCreationArgs(maxFlows)));
    water->GetTransform().IncrementPosition(Vector3f(0.0f, 0.0f, -10.0f));

    water->UpdateUniformLocations(waterMat);
    water->Params.Texture2DUniforms[texSamplerName] =
        UniformSampler2DValue(waterNormalTex1.GetTextureHandle(), texSamplerName,
                              waterMat->GetUniforms().FindUniform(texSamplerName,
                                                                  waterMat->GetUniforms().Texture2DUniforms).Loc);
    water->Params.Texture2DUniforms[texSampler2Name] =
        UniformSampler2DValue(waterNormalTex2.GetTextureHandle(), texSampler2Name,
                              waterMat->GetUniforms().FindUniform(texSampler2Name,
                                                                  waterMat->GetUniforms().Texture2DUniforms).Loc);
    water->Params.TextureCubemapUniforms[cubeSamplerName] =
        UniformSamplerCubemapValue(cubemapTex.GetTextureHandle(), cubeSamplerName,
                                   waterMat->GetUniforms().FindUniform(cubeSamplerName,
                                                                       waterMat->GetUniforms().TextureCubemapUniforms).Loc);

    water->AddFlow(Water::DirectionalWaterArgs(Vector2f(2.0f, 0.0f), 10.0f, 50.0f));


    //Cubemap.

    std::vector<PrimitiveGenerator::CubemapVertex> cmVertices;
    std::vector<unsigned int> cmIndices;
    PrimitiveGenerator::GenerateCubemapCube(cmVertices, cmIndices, false, true);

    RenderObjHandle cmVBO, cmIBO;
    RenderDataHandler::CreateVertexBuffer(cmVBO, cmVertices.data(), cmVertices.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(cmIBO, cmIndices.data(), cmIndices.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    cubemapMesh.SubMeshes.insert(cubemapMesh.SubMeshes.end(),
                                 VertexIndexData(cmVertices.size(), cmVBO, cmIndices.size(), cmIBO));


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
	if (IsGameOver()) return;

    std::string err = InitializeStaticSystems(false, true, true);
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
    waterNormalTex1.DeleteIfValid();
    waterNormalTex2.DeleteIfValid();
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
    if (!waterMat->Render(info, meshList, water->Params))
    {
        std::cout << "Error rendering water: " << waterMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    //Geometry shader test.
    meshList.clear();
    meshList.insert(meshList.end(), &gsMesh);
    if (!gsTestMat->Render(info, meshList, gsTestParams))
    {
        std::cout << "Error rendering geometry shader test: " << gsTestMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    //Particles.
    meshList.clear();
    meshList.insert(meshList.end(), &particleMesh);
    if (!particleMat->Render(info, meshList, particleParams))
    {
        std::cout << "Error rendering particles: " << particleMat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    //Cubemap.
    meshList.clear();
    meshList.insert(meshList.end(), &cubemapMesh);
    if (!cubemapMat->Render(info, meshList, cubemapParams))
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
	cam.GetPerspectiveTransform(projM);
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
    if (!ppc->RenderChain(this, cam.PerspectiveInfo, worldRendTex, (*RenderTargets)[worldRenderID]->GetDepthTexture().MTex->GetTextureHandle()))
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
    if (!finalScreenQuad->Render(RenderInfo(this, &cam, &trans, &identity, &identity, &identity), finalScreenQuadParams, *finalScreenMat))
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
	cam.PerspectiveInfo.Width = (float)newW;
	cam.PerspectiveInfo.Height = (float)newH;
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