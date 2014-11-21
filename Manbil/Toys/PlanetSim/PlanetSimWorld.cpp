#include "PlanetSimWorld.h"

#include <iostream>
#include "../../Math/NoiseGeneration.hpp"
#include "../../NoiseToTexture.h"
#include "../../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../../IO/XmlSerialization.h"
#include "../../ScreenClearer.h"
#include "PlanetSimWorldGen.h"



const std::string planetTex3DName = "u_planetTex3D",
                  planetTexHeightName = "u_planetHeightTex";


PlanetSimWorld::PlanetSimWorld(void)
    : windowSize(800, 600), SFMLOpenGLWorld(800, 600, sf::ContextSettings(24, 0, 0, 4, 1)),
      planetHeightTex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_32F, false), planetMat(0),
      planetTex3D(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PS_32F_GREYSCALE, true),
      cam(Vector3f(-6000.0f, -6000.0f, -6000.0f), 400.0f, 0.025f, 1.65f, Vector3f(1.0f, 1.0f, 0.0f).Normalized()),
      world(0)
{

}

PlanetSimWorld::~PlanetSimWorld(void)
{
    DeleteAndSetToNull(planetMat);
}


ShaderGenerator::GeneratedMaterial GenerateMaterial(UniformDictionary & params)
{
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

    //Write out the material to the file.
    SerializedMaterial ser(DataNode::VertexIns, DataNode::MaterialOuts);
    XmlWriter xWriter("planetMaterial");
    std::string error;
    if (!xWriter.WriteDataStructure(ser, "Planet Material", error))
        return ShaderGenerator::GeneratedMaterial("Error writing planet material data: " + error);
    error = xWriter.SaveData("Content/Materials/PlanetGen.xml");
    if (!error.empty())
        return ShaderGenerator::GeneratedMaterial("Error saving planet material file: " + error);


    return ShaderGenerator::GenerateMaterial(params, RenderingModes::RM_Opaque);
}
ShaderGenerator::GeneratedMaterial LoadMaterial(UniformDictionary & params)
{
    ShaderGenerator::GeneratedMaterial genM("Unknown error");


    XmlReader reader("Content/Materials/PlanetGen.xml", genM.ErrorMessage);
    if (!genM.ErrorMessage.empty())
    {
        genM.ErrorMessage = "Error loading file 'Content/Materials/PlanetGen.xml': " + genM.ErrorMessage;
        return genM;
    }
    

    SerializedMaterial ser;
    if (!reader.ReadDataStructure(ser, genM.ErrorMessage))
    {
        genM.ErrorMessage = "Error reading serialized material: " + genM.ErrorMessage;
        return genM;
    }


    DataNode::ClearMaterialData();
    DataNode::VertexIns = ser.VertexInputs;
    DataNode::MaterialOuts = ser.MaterialOuts;

    return ShaderGenerator::GenerateMaterial(params, RenderingModes::RM_Opaque);
}


const float startingZFar = 10000.0f;

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
    gradient.GetColors(heightTex.GetArray(), heightTexInput.GetArray(), heightTex.GetNumbElements());

    planetHeightTex.Create();
    if (!planetHeightTex.SetColorData(heightTex))
    {
        PrintError("Unable to set data for planet's heightmap texture.");
        return;
    }


    #pragma endregion
    

    #pragma region Materials

    //ShaderGenerator::GeneratedMaterial genM = GenerateMaterial(planetParams);
    ShaderGenerator::GeneratedMaterial genM = LoadMaterial(planetParams);
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
    

    world = new WorldData(513, 10000.0f, 10100.0f);


    //Camera data.
    cam.LockUp = false;
    cam.Window = GetWindow();
    cam.Info.Width = windowSize.x;
    cam.Info.Height = windowSize.y;
    cam.Info.zNear = 0.1f;
    cam.Info.zFar = startingZFar;
    cam.Info.FOV = ToRadian(60.0f);
    
}
void PlanetSimWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(planetMat);
    DeleteAndSetToNull(world);
}


void PlanetSimWorld::UpdateWorld(float elapsed)
{
    if (cam.Update(elapsed))
        EndWorld();

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
            planetParams.Texture3DUniforms[planetTex3DName].Texture = planetTex3D.GetTextureHandle();
            planetParams.Texture2DUniforms[planetTexHeightName].Texture = planetHeightTex.GetTextureHandle();
        }
    }

    //Lower the camera's zFar as it gets closer to the surface.
    const float maxHeightSpeed = world->GetMaxHeight() * 1.15f;
    const float minZFar = 1200.0f;
    float zFarLerp = BasicMath::Clamp(BasicMath::LerpComponent(world->GetMinHeight(), maxHeightSpeed, cam.GetPosition().Length()),
                                      0.0f, 1.0f);
    cam.Info.zFar = BasicMath::Lerp(minZFar, startingZFar, zFarLerp);
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
    world->GetMeshes(planetMeshList, cam.GetPosition(), cam.GetForward());

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