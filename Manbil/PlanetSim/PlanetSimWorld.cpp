#include "PlanetSimWorld.h"

#include <iostream>
#include "../Math/NoiseGeneration.hpp"
#include "../NoiseToTexture.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../ScreenClearer.h"
#include "PlanetSimWorldGen.h"



const std::string PlanetSimWorld::planetTex3DName = "u_planetTex3D";


PlanetSimWorld::PlanetSimWorld(void)
    : windowSize(800, 600), SFMLOpenGLWorld(800, 600, sf::ContextSettings(24, 0, 0, 4, 1)),
      planetTex3D(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PS_32F, true), planetMat(0),
      cam(Vector3f(-500.0f, 0.0f, 0.0f), 500.0f, 0.025f)
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

    Array3D<float> planetTexNoiseDat(100, 100, 100);
    Array3D<Vector4f> planetTexData(100, 100, 100);

    Perlin3D planetTexNoise(10.0f, Perlin3D::Smoothness::Quintic, Vector3i(), 12345, true, Vector3u(10, 10, 10));
    planetTexNoise.Generate(planetTexNoiseDat);

    ColorGradient plTexGrad;
    std::vector<ColorNode> & nodes = plTexGrad.OrderedNodes;
    nodes.insert(nodes.end(), ColorNode(0.0f, Vector4f(0.2f, 0.6f, 0.2f, 1.0f)));
    nodes.insert(nodes.end(), ColorNode(1.0f, Vector4f(0.6f, 0.2f, 0.2f, 1.0f)));
    plTexGrad.GetColors(planetTexData.GetArray(), planetTexNoiseDat.GetArray(), planetTexData.GetNumbElements());

    planetTex3D.Create();
    if (!planetTex3D.SetColorData(planetTexData))
    {
        PrintError("Unable to set data for planet's 3D texture.");
        return;
    }

    #pragma endregion
    

    #pragma region Materials


    //Material channels.

    std::unordered_map<RenderingChannels, DataLine> plChans;

    DataNodePtr vertexIns(new VertexInputNode(PlanetVertex::GetAttributeData()));
    DataNodePtr fragmentIns(new FragmentInputNode(PlanetVertex::GetAttributeData()));

    DataLine tex3DUVScale(0.01f);
    DataLine tex3DUVs(DataNodePtr(new MultiplyNode(DataLine(fragmentIns, 0), tex3DUVScale)), 0);
    DataLine tex3D(DataNodePtr(new TextureSample3DNode(tex3DUVs, planetTex3DName)),
                   TextureSample3DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    DataLine lightDir(VectorF(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()));
    DataLine lighting(DataNodePtr(new LightingNode(DataLine(fragmentIns, 0), DataLine(fragmentIns, 1), lightDir,
                      DataLine(0.2f), DataLine(0.8f), DataLine(0.0f))), 0);

    plChans[RenderingChannels::RC_VertexPosOutput] = DataLine(DataNodePtr(new ObjectPosToScreenPosCalcNode(DataLine(vertexIns, 0))),
                                                              ObjectPosToScreenPosCalcNode::GetHomogenousPosOutputIndex());
    plChans[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(vertexIns, 0);
    plChans[RenderingChannels::RC_VERTEX_OUT_1] = DataLine(vertexIns, 1);
    plChans[RenderingChannels::RC_VERTEX_OUT_2] = DataLine(vertexIns, 2);
    plChans[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new MultiplyNode(lighting, tex3D)), 0);

    //Material generation.
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(plChans, planetParams, PlanetVertex::GetAttributeData(),
                                                                                RenderingModes::RM_Opaque, true, LightSettings(false));
    if (!genM.ErrorMessage.empty())
    {
        PrintError("Error generating planet terrain material: " + genM.ErrorMessage);
        return;
    }
    planetMat = genM.Mat;

    //Material parameters.
    planetParams.Texture3DUniforms[planetTex3DName].Texture = planetTex3D.GetTextureHandle();


    #pragma endregion
    

    #pragma region Objects

    Array2D<float> noise(1024, 1024);


    #pragma region Generate the heightmap


    Perlin2D perlins[] =
    {
        Perlin2D(128.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 128, 99999)),
        Perlin2D(64.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 64, 99999)),
        Perlin2D(32.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 32, 99999)),
        Perlin2D(16.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 16, 99999)),
        Perlin2D(8.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 8, 99999)),
        Perlin2D(4.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 4, 99999)),
        Perlin2D(2.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 2, 99999)),
        Perlin2D(1.0f, Perlin2D::Quintic, Vector2i(), 123639, true, Vector2u(noise.GetWidth() / 1, 99999)),
    };
    Generator2D * gens[] = { &perlins[0], &perlins[1], &perlins[2], &perlins[3], &perlins[4], &perlins[5], &perlins[6], &perlins[7] };
    float weights[] = { 0.65f, 0.18f, 0.09f, 0.045f, 0.03125f, 0.006875f, 0.0037775f, 0.00171875f };
    LayeredOctave2D layered(sizeof(weights) / sizeof(float), weights, gens);

    FlatNoise2D floorNoise(0.15f);
    Combine2Noises2D floorFunc(&Combine2Noises2D::Max2, &layered, &floorNoise);

    NoiseFilterer2D filter;
    RectangularFilterRegion rfr(Vector2i(0, noise.GetHeight() - 5), Vector2i(noise.GetWidth(), noise.GetHeight() + 4), 0.65f, Interval(0.0f, 2.1f), true);
    filter.FillRegion = &rfr;
    filter.NoiseToFilter = &floorFunc;
    filter.FilterFunc = &NoiseFilterer2D::Average;

    filter.Generate(noise);


    #pragma endregion


    //Generate the planet.
    const float minHeight = 0.9f;
    const float heightScale = 0.2f;
    planetMeshes.GeneratePlanet(noise, 3000.0f, minHeight, heightScale, Vector2u(1024, 1024));

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

    if (!planetMat->Render(RenderPasses::BaseComponents, camInfo, planetMeshList, planetParams))
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