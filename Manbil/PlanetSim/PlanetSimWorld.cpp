#include "PlanetSimWorld.h"

#include <iostream>
#include "../Math/NoiseGeneration.hpp"
#include "../NoiseToTexture.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../ScreenClearer.h"
#include "PlanetSimWorldGen.h"



const std::string PlanetSimWorld::planetTex3DName = "u_planetTex3D",
                  PlanetSimWorld::planetTexHeightName = "u_planetHeightTex";


PlanetSimWorld::PlanetSimWorld(void)
    : windowSize(800, 600), SFMLOpenGLWorld(800, 600, sf::ContextSettings(24, 0, 0, 4, 1)),
      planetHeightTex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_32F, false), planetMat(0),
      planetTex3D(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PS_32F_GREYSCALE, true),
      cam(Vector3f(8000.0f, 0.0f, 0.0f), 400.0f, 0.025f, Vector3f(-1.0f, 0.0f, 0.0f))
{

}

PlanetSimWorld::~PlanetSimWorld(void)
{
    DeleteAndSetToNull(planetMat);
}


void PlanetSimWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    std::string error = InitializeStaticSystems(true, true, true);
    if (!error.empty())
    {
        PrintError("Error initializing static systems: " + error);
        return;
    }

    GetWindow()->setVerticalSyncEnabled(true);
    GetWindow()->setMouseCursorVisible(true);

    glViewport(0, 0, windowSize.x, windowSize.y);

    
    #pragma region Textures


    ColorGradient gradient;
    std::vector<ColorNode> & nodes = gradient.OrderedNodes;

    //Planet's 3D texture.

    Array3D<float> planetTexNoiseDat(100, 100, 100);
    Array3D<Vector4f> planetTexData(100, 100, 100);

    Perlin3D planetTexNoise(10.0f, Perlin3D::Smoothness::Quintic, Vector3i(), 12345, true, Vector3u(10, 10, 10));
    planetTexNoise.Generate(planetTexNoiseDat);

    nodes.insert(nodes.end(), ColorNode(0.0f, Vector4f(0.8f, 0.8f, 0.8f, 1.0f)));
    nodes.insert(nodes.end(), ColorNode(1.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
    gradient.GetColors(planetTexData.GetArray(), planetTexNoiseDat.GetArray(), planetTexData.GetNumbElements());
    planetTexNoiseDat.FillFunc([&planetTexData](Vector3u loc, float * outVal) { *outVal = planetTexData[loc].x; });

    planetTex3D.Create();
    if (!planetTex3D.SetGreyscaleData(planetTexNoiseDat))
    {
        PrintError("Unable to set data for planet's 3D texture.");
        return;
    }

    //Heightmap texture.

    Array2D<Vector4f> heightTex(1024, 1);
    Array2D<float> heightTexInput(1024, 1);
    heightTexInput.FillFunc([](Vector2u loc, float * outVal) { *outVal = (float)loc.x / 1023.0f; });

    nodes.clear();
    if (true)
    {
        nodes.insert(nodes.end(), ColorNode(0.0f, Vector4f(0.2f, 0.8f, 0.2f, 1.0f)));
        nodes.insert(nodes.end(), ColorNode(0.4f, Vector4f(0.4f, 0.2f, 0.1f, 1.0f)));
        nodes.insert(nodes.end(), ColorNode(0.55f, Vector4f(0.3f, 0.1f, 0.0f, 1.0f)));
        nodes.insert(nodes.end(), ColorNode(0.75f, Vector4f(0.4f, 0.2f, 0.1f, 1.0f)));
        nodes.insert(nodes.end(), ColorNode(1.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
    }
    else
    {
        nodes.insert(nodes.end(), ColorNode(0.0f, Vector4f(0.35f, 0.1f, 0.0f, 1.0f)));
        nodes.insert(nodes.end(), ColorNode(1.0f, Vector4f(0.8f, 0.2f, 0.2f, 1.0f)));
    }
    gradient.GetColors(heightTex.GetArray(), heightTexInput.GetArray(), heightTex.GetArea());

    planetHeightTex.Create();
    if (!planetHeightTex.SetColorData(heightTex))
    {
        PrintError("Unable to set data for planet's heightmap texture.");
        return;
    }


    #pragma endregion
    

    #pragma region Materials


    DataNode::ClearMaterialData();
    DataNode::VertexIns = PlanetVertex::GetAttributeData();
    typedef DataNode::Ptr DNP;

    //Vertex shader.
    DNP objPosToScreen(new SpaceConverterNode(DataLine(VertexInputNode::GetInstanceName()),
                                              SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                              SpaceConverterNode::DT_POSITION, "objToScreenPos"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(objPosToScreen->GetName(), 1);
    std::vector<ShaderOutput> * vertOuts = &DataNode::MaterialOuts.VertexOutputs;
    vertOuts->insert(vertOuts->end(), ShaderOutput("vOut_Pos", DataLine(VertexInputNode::GetInstanceName())));
    vertOuts->insert(vertOuts->end(), ShaderOutput("vOut_Normal", DataLine(VertexInputNode::GetInstanceName(), 1)));
    vertOuts->insert(vertOuts->end(), ShaderOutput("vOut_Height", DataLine(VertexInputNode::GetInstanceName(), 2)));


    //Fragment shader.

    //3D greyscale texture.
    DataLine tex3DUVScale(0.02f);
    DNP tex3DUVs(new MultiplyNode(DataLine(FragmentInputNode::GetInstanceName()), tex3DUVScale, "tex3dUV"));
    DNP tex3DPtr(new TextureSample3DNode(DataLine(tex3DUVs->GetName()), planetTex3DName, "tex3D"));
    DataLine tex3D(tex3DPtr->GetName(), TextureSample3DNode::GetOutputIndex(ChannelsOut::CO_Red));

    //2D height-to-color texture.
    DNP texHeightmapUVs(new CombineVectorNode(DataLine(FragmentInputNode::GetInstanceName(), 2), DataLine(0.5f),
                                              "texHeightmapUV"));
    DNP texHeightmapPtr(new TextureSample2DNode(DataLine(texHeightmapUVs->GetName()), planetTexHeightName, "texHeight"));
    DataLine texHeightmap(texHeightmapPtr->GetName(), TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));

    //Combine the texture data.
    DNP finalTexColor(new MultiplyNode(texHeightmap, tex3D, "finalTexColor"));

    //Lighting.
    DataLine lightDir(VectorF(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()));
    DNP lightBrightness(new LightingNode(DataLine(FragmentInputNode::GetInstanceName()),
                                         DataLine(FragmentInputNode::GetInstanceName(), 1),
                                         lightDir, "lightBrightness",
                                         DataLine(0.2f), DataLine(0.8f), DataLine(0.0f)));

    //Combine lighting and texturing.
    DNP litTexColor(new MultiplyNode(DataLine(lightBrightness->GetName()), DataLine(finalTexColor->GetName())));

    //Calculate fog.
    DNP distFromPlayer(new DistanceNode(CameraDataNode::GetCamPos(), DataLine(FragmentInputNode::GetInstanceName()),
                                        "distToPlayer"));
    DNP distFromPlayerLerp(new DivideNode(DataLine(distFromPlayer->GetName()), ProjectionDataNode::GetZFar(), "distToPlayerLerp"));
    DNP planetFogLerp(new CustomExpressionNode("(1.0f - '0') * pow(clamp(1.0f * '1', 0.0f, 1.0f), 0.1f)", 1,
                                               DataLine(distFromPlayerLerp->GetName()), "planetFogLerp"));
    DataLine planetFogColor(VectorF(1.0f, 1.0f, 1.0f));
    DNP finalPlanetFog(new InterpolateNode(planetFogColor, DataLine(litTexColor->GetName()),
                                           DataLine(planetFogLerp->GetName()),
                                           InterpolateNode::IT_VerySmooth));
    DNP finalPlanetColor(new CombineVectorNode(finalPlanetFog, 1.0f, "finalPlanetColor"));

    //Color output.
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_FinalColor", finalPlanetColor));

    //Material generation.
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(planetParams, RenderingModes::RM_Opaque);
    if (!genM.ErrorMessage.empty())
    {
        PrintError("Error generating planet terrain material: " + genM.ErrorMessage);
        return;
    }
    planetMat = genM.Mat;

    //Material parameters.
    planetParams.Texture3DUniforms[planetTex3DName].Texture = planetTex3D.GetTextureHandle();
    planetParams.Texture2DUniforms[planetTexHeightName].Texture = planetHeightTex.GetTextureHandle();


    #pragma endregion
    

    #pragma region Objects


    Array2D<float> noise(1024, 1024);


    #pragma region Generate the heightmap


    //Base heightmap
    Perlin2D perlins[] =
    {
        Perlin2D(128.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 128, 99999)),
        Perlin2D(64.0f, Perlin2D::Quintic, Vector2i(), 345234, true, Vector2u(noise.GetWidth() / 64, 99999)),
        Perlin2D(32.0f, Perlin2D::Quintic, Vector2i(), 2362, true, Vector2u(noise.GetWidth() / 32, 99999)),
        Perlin2D(16.0f, Perlin2D::Quintic, Vector2i(), 98356, true, Vector2u(noise.GetWidth() / 16, 99999)),
        Perlin2D(8.0f, Perlin2D::Quintic, Vector2i(), 36, true, Vector2u(noise.GetWidth() / 8, 99999)),
        Perlin2D(4.0f, Perlin2D::Quintic, Vector2i(), 236, true, Vector2u(noise.GetWidth() / 4, 99999)),
        Perlin2D(2.0f, Perlin2D::Quintic, Vector2i(), 23452, true, Vector2u(noise.GetWidth() / 2, 99999)),
        Perlin2D(1.0f, Perlin2D::Quintic, Vector2i(), 23721, true, Vector2u(noise.GetWidth() / 1, 99999)),
    };
    Generator2D * gens[] = { &perlins[0], &perlins[1], &perlins[2], &perlins[3], &perlins[4], &perlins[5], &perlins[6], &perlins[7] };
    float weights[] = { 0.65f, 0.18f, 0.09f, 0.045f, 0.03125f, 0.006875f, 0.0037775f, 0.00171875f };
    LayeredOctave2D layered(sizeof(weights) / sizeof(float), weights, gens);

    //Use pow() to make the heightmap steeper.
    FlatNoise2D powNoise(1.25f);
    Combine2Noises2D powFunc(&NoiseFuncs::Pow2, &layered, &powNoise);

    //Create a floor.
    FlatNoise2D floorNoise(0.2f);
    Combine2Noises2D floorFunc(&NoiseFuncs::Max2, &powFunc, &floorNoise);

    //Add a little noise so that the floor isn't totally flat.
    Perlin2D miniNoise(8.0f, Perlin2D::Quintic, Vector2i(), 162361, true, Vector2u(8, 8));
    FlatNoise2D miniNoiseWeight(0.0055f);
    Combine3Noises2D addMiniNoise(&NoiseFuncs::MultiplyThenAdd, &miniNoise, &miniNoiseWeight, &floorFunc);

    //Average out the top and bottom of the planet so that the top vertex and bottom vertex don't look too out of place.
    NoiseFilterer2D filter;
    RectangularFilterRegion rfr(Vector2i(0, noise.GetHeight() - 5), Vector2i(noise.GetWidth(), noise.GetHeight() + 4), 0.8f, Interval(0.0f, 2.1f), true);
    filter.FillRegion = &rfr;
    filter.NoiseToFilter = &addMiniNoise;
    filter.FilterFunc = &NoiseFilterer2D::Average;

    filter.Generate(noise);


    #pragma endregion


    //Generate the planet.
    const float minHeight = 0.75f;
    const float heightScale = 0.2f;
    planetMeshes.GeneratePlanet(noise, 6000.0f, minHeight, heightScale, Vector2u(1024, 1024));


    #pragma endregion


    //Camera data.
    cam.Window = GetWindow();
    cam.Info.Width = windowSize.x;
    cam.Info.Height = windowSize.y;
    cam.Info.zNear = 0.1f;
    cam.Info.zFar = 10000.0f;
    cam.Info.FOV = ToRadian(60.0f);
    
}
void PlanetSimWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(planetMat);
}


void PlanetSimWorld::UpdateWorld(float elapsed)
{
    if (cam.Update(elapsed))
        EndWorld();
}
void PlanetSimWorld::RenderOpenGL(float elapsed)
{
    //Set up rendering info.
    TransformObject trns;
    Matrix4f worldM, viewM, projM;
    trns.GetWorldTransform(worldM);
    cam.GetViewTransform(viewM);
    projM.SetAsPerspProj(cam.Info);
    RenderInfo info(this, &cam, &trns, &worldM, &viewM, &projM);

    //Set up rendering state.
    RenderingState().EnableState();
    ScreenClearer().ClearScreen();
    glViewport(0, 0, windowSize.x, windowSize.y);

    //Render the world.
    RenderWorld(elapsed, info);
}

void PlanetSimWorld::RenderWorld(float elapsed, RenderInfo & camInfo)
{
    std::vector<const Mesh*> planetMeshList;
    planetMeshes.GetVisibleTerrain(cam.GetPosition(), planetMeshList);

    if (!planetMat->Render(camInfo, planetMeshList, planetParams))
    {
        PrintError("Error rendering planet terrain: " + planetMat->GetErrorMsg());
        return;
    }
}


void PlanetSimWorld::PrintError(std::string error)
{
    std::cout << error << "\nEnter anything to continue:\n";
    char dummy;
    std::cin >> dummy;

    EndWorld();
}
void PlanetSimWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    windowSize = Vector2u(newW, newH);

    cam.Info.Width = windowSize.x;
    cam.Info.Height = windowSize.y;
    glViewport(0, 0, windowSize.x, windowSize.y);
}