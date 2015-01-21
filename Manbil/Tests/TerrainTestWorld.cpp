#include "TerrainTestWorld.h"

#include "../Math/NoiseGeneration.hpp"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Math/Higher Math/Terrain.h"

#include "../Input/Input.hpp"

#include "../DebugAssist.h"

#include <iostream>


typedef TerrainTestWorld TTW;

const unsigned int INPUT_NextLOD = 1,
                   INPUT_PrevLOD = 2;


TTW::TerrainTestWorld(void)
    : terrMat(0), terrMesh(TriangleList), windowSize(800, 600),
      terrTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PixelSizes::PS_32F, true),
      cam(Vector3f(0.0f, 0.0f, 25.0f), 40.0f, 0.16f),
      SFMLOpenGLWorld(800, 600, sf::ContextSettings(24, 0, 0, 4, 1))
{

}
TTW::~TerrainTestWorld(void)
{

}

void TTW::GenerateTerrainLOD(const Terrain& terr, unsigned int lodLevel)
{
    std::vector<VertexPosTex1Normal> verts;
    std::vector<unsigned int> inds;
    terr.GenerateTrianglesFull<VertexPosTex1Normal>(verts, inds,
                                                    [](VertexPosTex1Normal& v) { return &v.Pos; },
                                                    [](VertexPosTex1Normal& v) { return &v.TexCoords; },
                                                    [](VertexPosTex1Normal& v) { return &v.Normal; },
                                                    100.0f, lodLevel);
    
    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer(vbo, verts.data(), verts.size(),
                                          RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(ibo, inds.data(), inds.size(),
                                         RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    VertexIndexData vid(verts.size(), vbo, inds.size(), ibo);
    terrMesh.SubMeshes.insert(terrMesh.SubMeshes.end(), vid);
}

void TTW::InitializeTextures(void)
{
    terrTex.Create();

    //Try to load it from a file.
    std::string errorMsg;
    if (!Assert(terrTex.SetDataFromFile("Content/Textures/Grass.png", errorMsg),
                "Error loading 'Content/Textures/Grass.png'",
                errorMsg))
    {
        return;
    }
}
void TTW::InitializeMaterials(void)
{
    DataNode::ClearMaterialData();

    std::vector<ShaderOutput> &vertOuts = DataNode::MaterialOuts.VertexOutputs,
                              &fragOuts = DataNode::MaterialOuts.FragmentOutputs;

    //Vertex shader is a simple object-to-screen-space conversion.
    //It outputs world position, UV, and world normal to the fragment shader.

    DataNode::VertexIns = VertexPosTex1Normal::GetAttributeData();

    DataLine vIn_ObjPos(VertexInputNode::GetInstance(), 0),
             vIn_UV(VertexInputNode::GetInstance(), 1),
             vIn_ObjNormal(VertexInputNode::GetInstance(), 2);

    DataNode::Ptr objPosToWorld = SpaceConverterNode::ObjPosToWorldPos(vIn_ObjPos, "objPosToWorld");
    DataLine vOut_WorldPos(objPosToWorld);
    DataNode::Ptr vOut_WorldNormal(new SpaceConverterNode(vIn_ObjNormal,
                                                          SpaceConverterNode::ST_OBJECT,
                                                          SpaceConverterNode::ST_WORLD,
                                                          SpaceConverterNode::DT_NORMAL,
                                                          "objNormalToWorld"));
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_WorldPos", vOut_WorldPos));
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_UV", vIn_UV));
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_WorldNormal", vOut_WorldNormal));

    DataNode::Ptr objPosToScreen = SpaceConverterNode::ObjPosToScreenPos(vIn_ObjPos, "objPosToScreen");
    DataNode::MaterialOuts.VertexPosOutput = DataLine(objPosToScreen, 1);


    //Fragment shader combines the grass texture with ambient+diffuse lighting.

    DataLine fIn_WorldPos(FragmentInputNode::GetInstance(), 0),
             fIn_UV(FragmentInputNode::GetInstance(), 1),
             fIn_WorldNormal(FragmentInputNode::GetInstance(), 2);
    DataNode::Ptr normalizedNormal(new NormalizeNode(fIn_WorldNormal, "normalizedNormal"));
    DataLine lightDir(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
             ambientLight(0.5f),
             diffuseLight(0.5f),
             specLight(0.0f),
             specIntensity(256.0f);
    DataLine texScaleDown(513.0f);
    
    DataNode::Ptr finalUV(new MultiplyNode(fIn_UV, texScaleDown, "finalUV"));
    DataNode::Ptr texSamplePtr(new TextureSample2DNode(finalUV, "u_tex", "texSampler"));
    DataLine texSample(texSamplePtr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));

    DataNode::Ptr lightCalc(new LightingNode(fIn_WorldPos, normalizedNormal, lightDir,
                                             "lightCalc", ambientLight, diffuseLight,
                                             specLight, specIntensity));
    DataLine lightBrightness(lightCalc);

    DataNode::Ptr finalColor3(new MultiplyNode(lightBrightness, texSample, "finalColor3")),
                  finalColor4(new CombineVectorNode(finalColor3, 1.0f, "finalColor4"));
    fragOuts.insert(fragOuts.end(), ShaderOutput("fOut_FinalColor4", finalColor4));


    //Generate the final material.
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(terrParams, RM_Opaque);
    if (Assert(genM.ErrorMessage.empty(), "Error generating terrain shaders", genM.ErrorMessage))
    {
        terrMat = genM.Mat;
    }


    //Set up parameters.
    terrParams.Texture2DUniforms["u_tex"].Texture = terrTex.GetTextureHandle();
}
void TTW::InitializeObjects(void)
{
    //Generate the terrain heightmap using layers of perlin noise.

    Perlin2D pers[] =
    {
        Perlin2D(128.0f, Perlin2D::Quintic, Vector2i(), 166234, true),
        Perlin2D(64.0f, Perlin2D::Quintic, Vector2i(), 6543, true),
        Perlin2D(32.0f, Perlin2D::Quintic, Vector2i(), 666778, true),
        Perlin2D(16.0f, Perlin2D::Quintic, Vector2i(), 44, true),
        Perlin2D(8.0f, Perlin2D::Quintic, Vector2i(), 3356, true),
        Perlin2D(4.0f, Perlin2D::Quintic, Vector2i(), 3356, true),
        Perlin2D(2.0f, Perlin2D::Quintic, Vector2i(), 3356, true),
        Perlin2D(1.0f, Perlin2D::Quintic, Vector2i(), 3356, true),
    };
    Generator2D*const gens[] = 
    {
        &pers[0],
        &pers[1],
        &pers[2],
        &pers[3],
        &pers[4],
        &pers[5],
        &pers[6],
        &pers[7],
    };

    std::vector<float> weights;
    for (unsigned int i = 0; i < (sizeof(pers) / sizeof(Perlin2D)); ++i)
        weights.insert(weights.end(), 1.0f / powf(2.0f, (float)(i + 1)));

    LayeredOctave2D layeredPerlin(sizeof(pers) / sizeof(Perlin2D), weights.data(), gens);

    Noise2D outNoise(513, 513);
    layeredPerlin.Generate(outNoise);


    //Generate the terrain vertices using the heightmap.

    Terrain terr(outNoise.GetDimensions());
    terr.SetHeightmap(outNoise);
    
    GenerateTerrainLOD(terr, 0);
    GenerateTerrainLOD(terr, 1);
    GenerateTerrainLOD(terr, 2);
    GenerateTerrainLOD(terr, 3);
    GenerateTerrainLOD(terr, 4);
    GenerateTerrainLOD(terr, 5);
    GenerateTerrainLOD(terr, 6);

    terrMesh.Transform.SetScale(1.0f);
}
void TTW::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    if (IsGameOver()) return;

    InitializeStaticSystems(false, false, false);

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();
    
    Input.AddBoolInput(INPUT_NextLOD,
                       BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Right,
                                                                      BoolInput::JustPressed)));
    Input.AddBoolInput(INPUT_PrevLOD,
                       BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Left,
                                                                      BoolInput::JustPressed)));

    cam.Window = GetWindow();
    cam.PerspectiveInfo.SetFOVDegrees(55.0f);
    cam.PerspectiveInfo.zFar = 4000.0f;
}

void TTW::OnWorldEnd(void)
{
    delete terrMat;
    terrTex.DeleteIfValid();

    DestroyStaticSystems(false, false, false);
}

void TTW::UpdateWorld(float elapsedSeconds)
{
    if (cam.Update(elapsedSeconds))
    {
        EndWorld();
        return;
    }

    if (Input.GetBoolInputValue(INPUT_NextLOD) &&
        terrMesh.CurrentSubMesh < terrMesh.SubMeshes.size() - 1)
    {
        terrMesh.CurrentSubMesh += 1;
    }
    if (Input.GetBoolInputValue(INPUT_PrevLOD) &&
        terrMesh.CurrentSubMesh > 0)
    {
        terrMesh.CurrentSubMesh -= 1;
    }
}

bool TTW::RenderWorldGeometry(const RenderInfo& info)
{
    ScreenClearer().ClearScreen();
    RenderingState(true, true, RenderingState::C_NONE).EnableState();

    std::vector<const Mesh*> toDraw;
    toDraw.insert(toDraw.end(), &terrMesh);

    return terrMat->Render(info, toDraw, terrParams);
}
void TTW::RenderOpenGL(float elapsedSeconds)
{
    glViewport(0, 0, windowSize.x, windowSize.y);

    Matrix4f worldM, viewM, projM;
    TransformObject dummy;

    cam.GetViewTransform(viewM);
    cam.GetPerspectiveTransform(projM);

    RenderInfo info(this, &cam, &dummy, &worldM, &viewM, &projM);
    if (!Assert(RenderWorldGeometry(info), "Error rendering world", terrMat->GetErrorMsg()))
    {
        return;
    }
}

void TTW::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	std::cout << "Enter any key to continue:\n";
    char dummy;
    std::cin >> dummy;
}

void TTW::OnWindowResized(unsigned int newW, unsigned int newH)
{
    cam.PerspectiveInfo.Width = newW;
    cam.PerspectiveInfo.Height = newH;

    glViewport(0, 0, newW, newH);

    windowSize.x = newW;
    windowSize.y = newH;
}

bool TTW::Assert(bool test, std::string errorIntro, const std::string& error)
{
    if (!test)
    {
        std::cout << errorIntro << ": " << error << "\nEnter anything to continue: ";
        char dummy;
        std::cin >> dummy;

        EndWorld();
    }

    return test;
}