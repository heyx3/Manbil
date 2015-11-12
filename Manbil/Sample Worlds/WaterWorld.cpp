#include "WaterWorld.h"

#include <iostream>

#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/Primitives/PrimitiveGenerator.h"
#include "../Input/Input Objects/MouseBoolInput.h"


//An arbitrary ID for the input manager corresponding to the "add ripple" input.
const unsigned int INPUT_ADD_RIPPLE = 666;


//Information about the water.

const int waterVerticesPerSide = 100;
const Vector3f waterScale(1.0f, 1.0f, 1.0f);

const unsigned int maxRipples = 4;

const Water::DirectionalWaterArgs flows[] = {
    Water::DirectionalWaterArgs(Vector2f(waterVerticesPerSide * 0.45f,
                                         waterVerticesPerSide * 0.65f) * 0.1f,
                                0.1f, 1.0f),
};
const unsigned int nFlows = sizeof(flows) / sizeof(Water::DirectionalWaterArgs);


WaterWorld::WaterWorld(void)
    : windowSize(800, 600), fastRand(rand()),
      waterObj(0), waterMat(0), skyboxMat(0),
      normalMap1(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, false),
      normalMap2(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, false),
      skyboxTex(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, false),
      SFMLOpenGLWorld(800, 600)
{
    Input.AddBoolInput(INPUT_ADD_RIPPLE,
                       BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Left,
                                                                   BoolInput::JustPressed)));
}

sf::ContextSettings WaterWorld::GenerateContext(void)
{
    return sf::ContextSettings(24, 0, 0, 4, 1);
}

sf::VideoMode WaterWorld::GetModeToUse(unsigned int windowW, unsigned int windowH)
{
    //Change this return value to change the window resolution mode.
    //To use native fullscreen, return "sf::VideoMode::getFullscreenModes()[0];".
    return sf::VideoMode(windowW, windowH);
}
std::string WaterWorld::GetWindowTitle(void)
{
    //Change this to change the string on the window's title-bar
    //    (assuming it has a title-bar).
    return "Water World";
}
sf::Uint32 WaterWorld::GetSFStyleFlags(void)
{
    //Change this to change the properties of the window.
    return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
}

void WaterWorld::InitializeTextures(void)
{
    std::string errorMsg;

    //Load the water's normal maps.
    normalMap1.Create();
    if (!normalMap1.SetDataFromFile("Content/Sample Worlds/normalMap1.png", errorMsg))
    {
        std::cout << "Error loading first normal map: " << errorMsg <<
                     "\nEnter any character to continue: ";
        
        char dummy;
        std::cin >> dummy;
        
        EndWorld();
        return;
    }
    normalMap2.Create();
    if (!normalMap2.SetDataFromFile("Content/Sample Worlds/normalMap2.png", errorMsg))
    {
        std::cout << "Error loading second normal map: " << errorMsg <<
                     "\nEnter any character to continue: ";
        
        char dummy;
        std::cin >> dummy;
        
        EndWorld();
        return;
    }

    //Load the skybox cube map.
    //This cubemap was generated with the amazing, free program "Space Engine".
    std::string filePathStart = "Content/Sample Worlds/Skybox/Space_";
    skyboxTex.Create();
    errorMsg = skyboxTex.SetDataFromFiles(filePathStart + "Neg_X.jpg", filePathStart + "Neg_Y.jpg",
                                          filePathStart + "Neg_Z.jpg",
                                          filePathStart + "Pos_X.jpg", filePathStart + "Pos_Y.jpg",
                                          filePathStart + "Pos_Z.jpg");
    if (!errorMsg.empty())
    {
        std::cout << "Error loading cubemap textures: " << errorMsg <<
                     "\nEnter any character to continue: ";

        char dummy;
        std::cin >> dummy;

        EndWorld();
        return;
    }
}
void WaterWorld::InitializeMaterials(void)
{
    {
        #pragma region Skybox material

        SerializedMaterial matData;

        RenderIOAttributes cubemapVertIns = PrimitiveGenerator::CubemapVertex::GetVertexAttributes();
        matData.VertexInputs = cubemapVertIns;

        //The vertex shader is a standard shader that uses the WVP matrix.
        //It outputs the object-space position (from {-1, -1, -1} to {1, 1, 1}) to the fragment shader.
        DataLine vIn_Pos(VertexInputNode::GetInstance(), 0),
                 vIn_Normal(VertexInputNode::GetInstance(), 1);
        DataNode::Ptr objPosToScreenSpacePtr = SpaceConverterNode::ObjPosToScreenPos(vIn_Pos,
                                                                                     "objPosToScreen");
        matData.MaterialOuts.VertexPosOutput = DataLine(objPosToScreenSpacePtr, 1);
        matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_UV", vIn_Pos));

        //The fragment shader just outputs the cubemap's color.
        DataLine fIn_UV(FragmentInputNode::GetInstance(), 0);
        DataNode::Ptr cubemapTex(new TextureSampleCubemapNode(fIn_UV, "u_skyboxTex", "cubemapTexNode"));
        DataLine cubemapTexRGB(cubemapTex, TextureSampleCubemapNode::GetOutputIndex(CO_AllColorChannels));
        DataNode::Ptr finalColor(new CombineVectorNode(cubemapTexRGB, 1.0f, "finalColorNode"));
        matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_FinalColor", finalColor));

        //Compile the material.
        ShaderGenerator::GeneratedMaterial genM =
            ShaderGenerator::GenerateMaterial(matData, skyboxParams, BlendMode::GetOpaque());
        if (!genM.ErrorMessage.empty())
        {
            std::cout << "Error compiling skybox material: " << genM.ErrorMessage <<
                         "\nEnter anything to continue: ";

            char dummy;
            std::cin >> dummy;

            EndWorld();
            return;
        }
        skyboxMat = genM.Mat;

        //Set the skybox texture.
        skyboxParams.TextureCubemaps["u_skyboxTex"].Texture = skyboxTex.GetTextureHandle();

        #pragma endregion
    }

    {
        #pragma region Water material
         
        SerializedMaterial matData;

        RenderIOAttributes waterVertIns = WaterVertex::GetVertexAttributes();
        matData.VertexInputs = waterVertIns;

        //The water system gives us a "WaterNode", which handles water simulation.
        //This node will be used in both the vertex and fragment shader.

        //The vertex shader takes the model-space vertices, distorts them according to a "WaterNode"'s
        //    output, then converts them to screen-space.
        //It outputs the object-space position, world-space position,
        //    and UV coordinates of the vertices.
        DataLine vIn_ObjPos(VertexInputNode::GetInstance(), 0),
                 vIn_UV(VertexInputNode::GetInstance(), 1);

        //The fragment shader gets the normal of the water surface from the "WaterNode",
        //    then adds in two normal maps to add fake bumps to the surface.
        DataLine fIn_ObjPos(FragmentInputNode::GetInstance(), 0),
                 fIn_WorldPos(FragmentInputNode::GetInstance(), 1),
                 fIn_UV(FragmentInputNode::GetInstance(), 2);

        //Create the water node and its outputs.
        DataNode::Ptr waterPtr(new WaterNode(vIn_ObjPos, fIn_ObjPos, maxRipples, nFlows));
        DataLine waterObjPos(waterPtr, 0),
                 waterSurfaceNormal(waterPtr, 1);

        //Now make the vertex shader.
        DataNode::Ptr screenPos = SpaceConverterNode::ObjPosToScreenPos(waterObjPos),
                      worldPos = SpaceConverterNode::ObjPosToWorldPos(waterObjPos);
        matData.MaterialOuts.VertexPosOutput = DataLine(screenPos, 1);
        matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_ObjPos", waterObjPos));
        matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_WorldPos",
                                                                    DataLine(worldPos, 0)));
        matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_UV", vIn_UV));


        //The fragment shader is more complicated.

        //First, compute the normal of the water's surface: get both normal maps and apply them onto
        //    the water surface.
        //Note that textures store values from 0 to 1, but normal vector components range from -1 to 1,
        //    so the texture values have to be remapped to that range.
        const Vector2f normalMap1Scale(10.0f, 10.0f),
                       normalMap1Pan(0.03f, 0.03f),
                       normalMap2Scale(1.0f, 1.0f),
                       normalMap2Pan(-0.03f, 0.03f);
        const float normalMap1Severity = 0.4f,
                    normalMap2Severity = 0.2f;
        DataNode::Ptr normalMap1UV(new CustomExpressionNode("('0' * '1') + ('2' * '3')", 2,
                                                            TimeNode::GetInstance(), normalMap1Pan,
                                                            fIn_UV, normalMap1Scale)),
                      normalMap2UV(new CustomExpressionNode("('0' * '1') + ('2' * '3')", 2,
                                                            TimeNode::GetInstance(), normalMap2Pan,
                                                            fIn_UV, normalMap2Scale));
        DataNode::Ptr normalMap1Ptr(new TextureSample2DNode(normalMap1UV, "u_normalMap1")),
                      normalMap2Ptr(new TextureSample2DNode(normalMap2UV, "u_normalMap2"));
        DataLine normalMap1RGB(normalMap1Ptr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels)),
                 normalMap2RGB(normalMap2Ptr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
        DataNode::Ptr normal1(new RemapNode(normalMap1RGB, 0.0f, 1.0f, -1.0f, 1.0f)),
                      normal1Final(new MultiplyNode(normal1, normalMap1Severity)),
                      normal2(new RemapNode(normalMap2RGB, 0.0f, 1.0f, -1.0f, 1.0f)),
                      normal2Final(new MultiplyNode(normal2, normalMap2Severity));
        DataNode::Ptr tangentNormal(new TangentSpaceNormalsNode(normal1Final, normal2Final,
                                                                        waterSurfaceNormal));
        DataNode::Ptr worldNormal(new SpaceConverterNode(tangentNormal,
                                                         SpaceConverterNode::ST_OBJECT,
                                                         SpaceConverterNode::ST_WORLD,
                                                         SpaceConverterNode::DT_NORMAL));
        //TODO: Get WaterNode to also output tangent/bitangent and then use ApplyNormalMapNode.
        //DataNode::Ptr combinedNormal(new ApplyNormalMapNode(waterSurfaceNormal, waterSurfaceTangent,
        //                                                    waterSurfaceBitangent, tangentNormal,
        //                                                    "finalNormal"));

        //The water itself has a color, but it also reflects light from the skybox.
        //This material interpolates between the water color and the reflected skybox
        //    based on a "reflectivity" value.
        const Vector3f waterColor(0.1f, 0.15f, 1.0f);
        const float waterReflectivity = 0.5f;
        DataNode::Ptr camToPos(new SubtractNode(fIn_WorldPos, CameraDataNode::GetCamPos()));
        DataNode::Ptr reflectEye(new ReflectNode(camToPos, tangentNormal));
        DataNode::Ptr skyboxPtr(new TextureSampleCubemapNode(reflectEye, "u_skyboxCubeTex"));
        DataLine reflectedColor(skyboxPtr,
                                TextureSampleCubemapNode::GetOutputIndex(CO_AllColorChannels));
        DataNode::Ptr surfaceColor(new InterpolateNode(waterColor, reflectedColor, waterReflectivity,
                                                       InterpolateNode::IT_Linear));

        //Finally, add lighting effects. There is a "LightingNode" class that does basic
        //    ambient-diffuse-specular lighting computation.
        const Vector3f lightDir = Vector3f(1.0f, 1.0f, -0.3f).Normalized(),
                       lightColor = Vector3f(1.0f, 1.0f, 1.0f);
        const float ambientLight = 0.4f,
                    diffuseLight = 1.0f - ambientLight,
                    specularLight = 0.0f,
                    specularIntensity = 64.0f;
        DataNode::Ptr brightnessCalc(new LightingNode(fIn_WorldPos, tangentNormal, lightDir,
                                                      ambientLight, diffuseLight, specularLight,
                                                      specularIntensity));
        DataNode::Ptr litSurfaceColor(new MultiplyNode(surfaceColor, brightnessCalc));

        DataNode::Ptr outputColor(new CombineVectorNode(litSurfaceColor, 1.0f));
        matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color", outputColor));


        //Compile the material.
        ShaderGenerator::GeneratedMaterial genM =
            ShaderGenerator::GenerateMaterial(matData, waterObj->Params, BlendMode::GetOpaque());
        if (!genM.ErrorMessage.empty())
        {
            std::cout << "Error creating water material: " << genM.ErrorMessage <<
                         "\nEnter anything to continue: ";

            char dummy;
            std::cin >> dummy;

            EndWorld();
            return;
        }
        waterMat = genM.Mat;

        //Set the parameters.
        waterObj->Params.Texture2Ds["u_normalMap1"].Texture = normalMap1.GetTextureHandle();
        waterObj->Params.Texture2Ds["u_normalMap2"].Texture = normalMap2.GetTextureHandle();
        waterObj->Params.TextureCubemaps["u_skyboxCubeTex"].Texture = skyboxTex.GetTextureHandle();


        #pragma endregion
    }
}
void WaterWorld::InitializeObjects(void)
{
    waterObj->SetMaterial(waterMat);
    waterObj->GetTransform().SetScale(50.0f);

    //Add the specified flows.
    for (unsigned int i = 0; i < nFlows; ++i)
    {
        waterObj->AddFlow(flows[i]);
    }


    //Create the skybox mesh.
    std::vector<PrimitiveGenerator::CubemapVertex> skyboxVerts;
    std::vector<unsigned int> skyboxInds;
    PrimitiveGenerator::GenerateCubemapCube(skyboxVerts, skyboxInds, true, true);
    skyboxMesh.SubMeshes.push_back(MeshData(false, PrimitiveTypes::PT_TRIANGLE_LIST));
    skyboxMesh.SubMeshes[0].SetVertexData(skyboxVerts, MeshData::BUF_STATIC,
                                          PrimitiveGenerator::CubemapVertex::GetVertexAttributes());
    skyboxMesh.SubMeshes[0].SetIndexData(skyboxInds, MeshData::BUF_STATIC);

    //Scale up the skybox mesh.
    skyboxMesh.Transform.SetScale(1400.0f);
}
void WaterWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    //If there was an error initializing the game, don’t bother with
    //    the rest of initialization.
    if (IsGameOver())
    {
        return;
    }
    
    waterObj = new Water(waterVerticesPerSide, Vector3f(), waterScale, maxRipples, nFlows);

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();

    gameCam = MovingCamera(Vector3f(0.0f, 0.0f, 20.0f), 30.0f, 0.8f);
    gameCam.PerspectiveInfo.SetFOVDegrees(60.0f);
    gameCam.PerspectiveInfo.Width = windowSize.x;
    gameCam.PerspectiveInfo.Height = windowSize.y;
    gameCam.PerspectiveInfo.zNear = 0.1f;
    gameCam.PerspectiveInfo.zFar = 2500.0f;
    gameCam.Window = GetWindow();
}
void WaterWorld::OnWorldEnd(void)
{
    if (waterObj != 0)
    {
        delete waterObj;
    }
    if (waterMat != 0)
    {
        delete waterMat;
    }
    if (skyboxMat != 0)
    {
        delete skyboxMat;
    }
    
    normalMap1.DeleteIfValid();
    normalMap2.DeleteIfValid();
    skyboxTex.DeleteIfValid();
}

void WaterWorld::UpdateWorld(float elapsedSeconds)
{
    waterObj->Update(elapsedSeconds);
    gameCam.Update(elapsedSeconds);

    //Keep the skybox centered around the player.
    skyboxMesh.Transform.SetPosition(gameCam.GetPosition());

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
        return;
    }

    //If the left mouse button was pressed, add a randomized ripple.
    if (Input.GetBoolInputValue(INPUT_ADD_RIPPLE))
    {
        const float minDropoffPoint = 40.0f,
                    maxDropoffPoint = 200.0f;
        const float minAmplitude = 0.5f,
                    maxAmplitude = 1.75f;
        const float minPeriod = 0.25f,
                    maxPeriod = 10.0f;
        const float minSpeed = 3.5f,
                    maxSpeed = 10.0f;

        waterObj->AddRipple(Water::RippleWaterArgs(gameCam.GetPosition(),
                                                   Mathf::Lerp(minDropoffPoint, maxDropoffPoint,
                                                               fastRand.GetZeroToOne()),
                                                   Mathf::Lerp(minAmplitude, maxAmplitude,
                                                               fastRand.GetZeroToOne()),
                                                   Mathf::Lerp(minPeriod, maxPeriod,
                                                               fastRand.GetZeroToOne()),
                                                   Mathf::Lerp(minSpeed, maxSpeed,
                                                               fastRand.GetZeroToOne())));
    }
}
void WaterWorld::RenderOpenGL(float elapsedSeconds)
{
    //Set up rendering state.
    ScreenClearer(true, true, false, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_BACK).EnableState();
    Viewport(0, 0, windowSize.x, windowSize.y).Use();

    //Set up world render info.
    Matrix4f viewM, projM;
    gameCam.GetViewTransform(viewM);
    gameCam.GetPerspectiveProjection(projM);
    RenderInfo info(GetTotalElapsedSeconds(), &gameCam, &viewM, &projM);

    //Render the water.
    waterObj->Render(info);
    skyboxMat->Render(info, &skyboxMesh, skyboxParams);
}

void WaterWorld::OnInitializeError(std::string errorMsg)
{
    //Print the error message and end the game.
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    EndWorld();
}
void WaterWorld::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
    windowSize.x = newWidth;
    windowSize.y = newHeight;

    gameCam.PerspectiveInfo.Width = windowSize.x;
    gameCam.PerspectiveInfo.Height = windowSize.y;
}