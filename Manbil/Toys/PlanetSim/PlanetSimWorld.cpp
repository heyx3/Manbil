#include "PlanetSimWorld.h"

#include <iostream>
#include "../../Math/NoiseGeneration.hpp"
#include "../../Rendering/NoiseToTexture.h"
#include "../../Rendering/Data Nodes/DataNodes.hpp"
#include "../../Rendering/Data Nodes/ShaderGenerator.h"
#include "../../IO/XmlSerialization.h"
#include "../../Rendering/Basic Rendering/ScreenClearer.h"
#include "../../Rendering/Primitives/DrawingQuad.h"
#include "PlanetSimWorldGen.h"



const std::string planetTex3DName = "u_planetTex3D",
                  planetTexHeightName = "u_planetHeightTex";


PlanetSimWorld::PlanetSimWorld(void)
    : windowSize(800, 600), SFMLOpenGLWorld(800, 600, sf::ContextSettings(24, 0, 0, 4, 1)),
      planetHeightTex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_32F, false), planetMat(0),
      planetTex3D(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PS_32F_GREYSCALE, true),
      cam(Vector3f(6000.0f, 6000.0f, 6000.0f), 400.0f, 0.1f, 1.65f,
          Vector3f(-1.0f, -1.0f, 0.0f).Normalized()),
      world(0)
{

}


ShaderGenerator::GeneratedMaterial GenerateMaterial(UniformDictionary& params)
{
    DataNode::ClearMaterialData();
    DataNode::VertexIns = PlanetVertex::GetAttributeData();
    typedef DataNode::Ptr DNP;

    //Vertex shader.
    DNP objPosToScreen(new SpaceConverterNode(DataLine(VertexInputNode::GetInstanceName()),
                                              SpaceConverterNode::ST_OBJECT,
                                              SpaceConverterNode::ST_SCREEN,
                                              SpaceConverterNode::DT_POSITION,
                                              "objToScreenPos"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(objPosToScreen->GetName(), 1);
    std::vector<ShaderOutput> * vertOuts = &DataNode::MaterialOuts.VertexOutputs;
    vertOuts->push_back(ShaderOutput("vOut_Pos", DataLine(VertexInputNode::GetInstanceName())));
    vertOuts->push_back(ShaderOutput("vOut_Normal", DataLine(VertexInputNode::GetInstanceName(), 1)));
    vertOuts->push_back(ShaderOutput("vOut_Height", DataLine(VertexInputNode::GetInstanceName(), 2)));


    //Fragment shader.

    //Inputs.
    DataLine fIn_Pos(FragmentInputNode::GetInstance(), 0),
             fIn_Normal(FragmentInputNode::GetInstance(), 1),
             fIn_Height(FragmentInputNode::GetInstance(), 2);

    //3D greyscale texture.
    DataLine tex3DUVScale(0.02f);
    DNP tex3DUVs(new MultiplyNode(fIn_Pos, tex3DUVScale, "tex3dUV"));
    DNP tex3DPtr(new TextureSample3DNode(tex3DUVs, planetTex3DName, "tex3D"));
    DataLine tex3D(tex3DPtr, TextureSample3DNode::GetOutputIndex(ChannelsOut::CO_Red));

    //2D height-to-color texture.
    DNP texHeightmapUVs(new CombineVectorNode(fIn_Height, DataLine(0.5f), "texHeightmapUV"));
    DNP texHeightmapPtr(new TextureSample2DNode(texHeightmapUVs, planetTexHeightName, "texHeight"));
    DataLine texHeightmap(texHeightmapPtr,
                          TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));

    //Combine the texture data.
    DNP finalTexColor(new MultiplyNode(texHeightmap, tex3D, "finalTexColor"));

    //Lighting.
    DataLine lightDir(VectorF(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()));
    DNP lightBrightness(new LightingNode(fIn_Pos, fIn_Normal, lightDir, "lightBrightness",
                                         DataLine(0.2f), DataLine(0.8f), DataLine(0.0f)));

    //Combine lighting and texturing.
    DNP litTexColor(new MultiplyNode(lightBrightness, finalTexColor));

    //Calculate fog.
    DNP distFromPlayer(new DistanceNode(CameraDataNode::GetCamPos(), fIn_Pos, "distToPlayer"));
    DNP distFromPlayerLerp(new DivideNode(distFromPlayer, ProjectionDataNode::GetZFar(),
                                          "distToPlayerLerp"));
    DNP planetFogLerp(new CustomExpressionNode("(1.0f - '0') * pow(clamp(1.0f * '1', 0.0f, 1.0f), 0.1f)", 1,
                                               distFromPlayerLerp, "planetFogLerp"));
    DataLine planetFogColor(Vector3f(1.0f, 1.0f, 1.0f));
    DNP finalPlanetFog(new InterpolateNode(planetFogColor, litTexColor, planetFogLerp,
                                           InterpolateNode::IT_VerySmooth));
    DNP finalPlanetColor(new CombineVectorNode(finalPlanetFog, 1.0f, "finalPlanetColor"));

    //Color output.
    DataNode::MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_FinalColor", finalPlanetColor));


    //Write out the material to the file.
    SerializedMaterial ser(DataNode::VertexIns, DataNode::MaterialOuts);
    XmlWriter xWriter("planetMaterial");
    xWriter.WriteDataStructure(ser, "Planet material");
    std::string error = xWriter.SaveData("Content/Materials/PlanetGen.xml");
    if (!error.empty())
    {
        return ShaderGenerator::GeneratedMaterial("Error saving planet material file: " + error);
    }


    return ShaderGenerator::GenerateMaterial(params, BlendMode::GetOpaque());
}
ShaderGenerator::GeneratedMaterial LoadMaterial(UniformDictionary& params)
{
    ShaderGenerator::GeneratedMaterial genM("Unknown error");

    XmlReader reader("Content/Materials/PlanetGen.xml");
    if (!reader.ErrorMessage.empty())
    {
        genM.ErrorMessage = "Error loading file 'Content/Materials/PlanetGen.xml': " +
                                reader.ErrorMessage;
        return genM;
    }


    SerializedMaterial ser;
    try
    {
        reader.ReadDataStructure(ser);
    }
    catch (int e)
    {
        assert(e == DataReader::EXCEPTION_FAILURE);
        return ShaderGenerator::GeneratedMaterial("Error reading in planet material: " +
                                                  reader.ErrorMessage);
    }

    DataNode::ClearMaterialData();
    DataNode::VertexIns = ser.VertexInputs;
    DataNode::MaterialOuts = ser.MaterialOuts;

    return ShaderGenerator::GenerateMaterial(params, BlendMode::GetOpaque());
}


const float startingZFar = 10000.0f;

void PlanetSimWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    DrawingQuad::InitializeQuad();

    GetWindow()->setVerticalSyncEnabled(true);
    GetWindow()->setMouseCursorVisible(true);

    glViewport(0, 0, windowSize.x, windowSize.y);

    
    #pragma region Textures


    ColorGradient gradient;
    std::vector<ColorNode>& nodes = gradient.OrderedNodes;

    //Planet's 3D texture.

    Array3D<float> planetTexNoiseDat(100, 100, 100);
    Array3D<Vector4f> planetTexData(100, 100, 100);

    Perlin3D planetTexNoise(10.0f, Perlin3D::Smoothness::Quintic, Vector3i(),
                            12345, true, Vector3u(10, 10, 10));
    planetTexNoise.Generate(planetTexNoiseDat);

    nodes.push_back(ColorNode(0.0f, Vector4f(0.8f, 0.8f, 0.8f, 1.0f)));
    nodes.push_back(ColorNode(1.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
    gradient.GetColors(planetTexData.GetArray(), planetTexNoiseDat.GetArray(),
                       planetTexData.GetNumbElements());
    planetTexNoiseDat.FillFunc([&planetTexData](Vector3u loc, float* outVal)
                               {
                                   *outVal = planetTexData[loc].x;
                               });

    planetTex3D.Create();
    planetTex3D.SetGreyscaleData(planetTexNoiseDat);

    //Heightmap texture.

    Array2D<Vector4f> heightTex(1024, 1);
    Array2D<float> heightTexInput(1024, 1);
    heightTexInput.FillFunc([](Vector2u loc, float* outVal) { *outVal = (float)loc.x / 1023.0f; });

    nodes.clear();
    nodes.push_back(ColorNode(0.0f, Vector4f(0.2f, 0.8f, 0.2f, 1.0f)));
    nodes.push_back(ColorNode(0.4f, Vector4f(0.4f, 0.2f, 0.1f, 1.0f)));
    nodes.push_back(ColorNode(0.55f, Vector4f(0.3f, 0.1f, 0.0f, 1.0f)));
    nodes.push_back(ColorNode(0.75f, Vector4f(0.4f, 0.2f, 0.1f, 1.0f)));
    nodes.push_back(ColorNode(1.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
    gradient.GetColors(heightTex.GetArray(), heightTexInput.GetArray(), heightTex.GetNumbElements());

    planetHeightTex.Create();
    planetHeightTex.SetColorData(heightTex);


    #pragma endregion
    

    #pragma region Materials


    ShaderGenerator::GeneratedMaterial genM = GenerateMaterial(planetParams);
    //ShaderGenerator::GeneratedMaterial genM = LoadMaterial(planetParams);
    if (!genM.ErrorMessage.empty())
    {
        PrintError("Error generating planet terrain material: " + genM.ErrorMessage);
        return;
    }
    planetMat = genM.Mat;

    //Material parameters.
    planetParams.Texture3Ds[planetTex3DName].Texture = planetTex3D.GetTextureHandle();
    planetParams.Texture2Ds[planetTexHeightName].Texture = planetHeightTex.GetTextureHandle();


    #pragma endregion
    

    world = new WorldData(513, 10000.0f, 10100.0f);


    //Camera data.
    cam.LockUp = false;
    cam.Window = GetWindow();
    cam.PerspectiveInfo.Width = windowSize.x;
    cam.PerspectiveInfo.Height = windowSize.y;
    cam.PerspectiveInfo.zNear = 0.1f;
    cam.PerspectiveInfo.zFar = startingZFar;
    cam.PerspectiveInfo.SetFOVDegrees(60.0f);
    
}
void PlanetSimWorld::OnWorldEnd(void)
{
    delete planetMat;
    delete world;

    DrawingQuad::DestroyQuad();
}


void PlanetSimWorld::UpdateWorld(float elapsed)
{
    if (cam.Update(elapsed))
    {
        EndWorld();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        ShaderGenerator::GeneratedMaterial genM = LoadMaterial(planetParams);
        if (!genM.ErrorMessage.empty())
        {
            PrintError("Error generating planet terrain material: " + genM.ErrorMessage);
        }
        else
        {
            delete planetMat;
            planetMat = genM.Mat;

            //Material parameters.
            planetParams.Texture3Ds[planetTex3DName].Texture = planetTex3D.GetTextureHandle();
            planetParams.Texture2Ds[planetTexHeightName].Texture = planetHeightTex.GetTextureHandle();
        }
    }

    //Lower the camera's zFar as it gets closer to the surface.
    const float maxHeightSpeed = world->GetMaxHeight() * 1.15f;
    const float minZFar = 1200.0f;
    float zFarLerp = Mathf::Clamp(Mathf::LerpComponent(world->GetMinHeight(), maxHeightSpeed,
                                                       cam.GetPosition().Length()),
                                  0.0f, 1.0f);
    cam.PerspectiveInfo.zFar = Mathf::Lerp(minZFar, startingZFar, zFarLerp);
}
void PlanetSimWorld::RenderOpenGL(float elapsed)
{
    //Set up rendering info.
    Matrix4f viewM, projM;
    cam.GetViewTransform(viewM);
    cam.GetPerspectiveTransform(projM);
    RenderInfo info(GetTotalElapsedSeconds(), &cam, &viewM, &projM);

    //Set up rendering state.
    RenderingState(RenderingState::C_NONE).EnableState();
    ScreenClearer().ClearScreen();
    BlendMode::GetOpaque().EnableMode();
    glViewport(0, 0, windowSize.x, windowSize.y);

    //Render the world.
    RenderWorld(elapsed, info);
}

void PlanetSimWorld::RenderWorld(float elapsed, RenderInfo& camInfo)
{
    std::vector<const Mesh*> planetMeshList;
    world->GetMeshes(planetMeshList, cam.GetPosition(), cam.GetForward());

    planetMat->Render(camInfo, planetMeshList, planetParams);
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

    cam.PerspectiveInfo.Width = windowSize.x;
    cam.PerspectiveInfo.Height = windowSize.y;
    glViewport(0, 0, windowSize.x, windowSize.y);
}