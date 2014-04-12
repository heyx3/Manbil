#include "VoxelWorld.h"

#include "../Math/Higher Math/GeometricMath.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../ScreenClearer.h"
#include "../Math/Shapes/ThreeDShapes.h"
#include "../Input/Input.hpp"
#include "../Rendering/Materials/Data Nodes/Parameters/TextureSampleNode.h"


#include <iostream>
namespace VWErrors
{
    void PauseConsole(void)
    {
        std::cout << "Enter a key to continue: ";
        char dummy;
        std::cin >> dummy;
        std::cout << "\n\n";
    }
    void PrintError(std::string intro, std::string error)
    {
        std::cout << intro << ": " << error << "\n\n";
        PauseConsole();
    }
}
using namespace VWErrors;



Vector2i vWindowSize(400, 400);
const unsigned int INPUT_AddVoxel = 123753,
                   INPUT_Quit = 6666666,
                   INPUT_MouseCap = 1337;
bool capMouse = true;


VoxelWorld::VoxelWorld(void)
    : SFMLOpenGLWorld(vWindowSize.x, vWindowSize.y, sf::ContextSettings(8, 0, 0, 3, 1)),
        voxelMat(0), renderState(), voxelMesh(PrimitiveTypes::Triangles),
        light(0.7f, 0.3f, Vector3f(1, 1, 1), Vector3f(-1, -1, -10).Normalized()),
        player(manager)
{
}

void VoxelWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    //Input.
    Input.AddBoolInput(INPUT_AddVoxel, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Left, BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_Quit, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Escape, BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_MouseCap, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Space, BoolInput::ValueStates::JustPressed)));


    //Initialize the chunk mesh.
    VoxelChunk * chunk = GetCreateChunk(Vector3i());
    chunk->SetVoxelLocal(Vector3i(0, 0, 0), true);
    Sphere spher(Vector3f(15.0f, 15.0f, 15.0f), 5.0f);
    chunk->SetVoxels(spher, true);
    Capsule caps(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 15.0f), 3.0f);
    chunk->SetVoxels(caps, true);
    Cube cbe(Box3D(15.0f, 15.0f, 15.0f, Vector3f(15.0f, 15.0f, 15.0f)));
    chunk->ToggleVoxels(cbe);

    chunkMeshes[Vector3i()]->RebuildMesh(true);

    voxelMesh.SetVertexIndexData(chunkMeshes[Vector3i()]->GetVID());
    voxelMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_AmbientName].SetValue(light.AmbientIntensity);
    voxelMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DiffuseName].SetValue(light.DiffuseIntensity);
    voxelMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DirName].SetValue(light.Direction);
    voxelMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_ColorName].SetValue(light.Color);


    //Initialize the texture.
    voxelTex = Textures.CreateTexture("Content/Textures/VoxelTex.png");
    if (voxelTex == TextureManager::UNUSED_ID)
    {
        PrintError("Error creating voxel texture 'Content/Textures/VoxelTex.png", "File not found or unable to be loaded");
        EndWorld();
        return;
    }
    Textures[voxelTex].SetData(TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_CLAMP, true));


    //Initialize the material.
    std::unordered_map<RenderingChannels, DataLine> channels;
    channels[RenderingChannels::RC_Diffuse] = DataLine(DataNodePtr(new TextureSampleNode("u_voxelTex")), TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    channels[RenderingChannels::RC_Specular] = DataLine(2.0f);
    channels[RenderingChannels::RC_SpecularIntensity] = DataLine(128.0f);
    UniformDictionary dict;
    voxelMat = ShaderGenerator::GenerateMaterial(channels, dict, RenderingModes::RM_Opaque, true, LightSettings(false));
    if (voxelMat->HasError())
    {
        PrintError("Error creating voxel material", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }
    //TODO: Smoother mouse input. Make an Acceleration Vector2Input class?
    std::unordered_map<std::string, UniformValueF> & fUnis = voxelMesh.Uniforms.FloatUniforms;
    std::unordered_map<std::string, UniformSamplerValue> & tUnis = voxelMesh.Uniforms.TextureUniforms;
    const std::vector<UniformList::Uniform> & unis = voxelMat->GetUniforms(RenderPasses::BaseComponents).FloatUniforms;
    fUnis[MaterialConstants::DirectionalLight_AmbientName].Location = UniformList::FindUniform(MaterialConstants::DirectionalLight_AmbientName, unis).Loc;
    fUnis[MaterialConstants::DirectionalLight_DiffuseName].Location = UniformList::FindUniform(MaterialConstants::DirectionalLight_DiffuseName, unis).Loc;
    fUnis[MaterialConstants::DirectionalLight_ColorName].Location = UniformList::FindUniform(MaterialConstants::DirectionalLight_ColorName, unis).Loc;
    fUnis[MaterialConstants::DirectionalLight_DirName].Location = UniformList::FindUniform(MaterialConstants::DirectionalLight_DirName, unis).Loc;
    voxelMesh.Uniforms.TextureUniforms["u_voxelTex"] = UniformSamplerValue(Textures[voxelTex], "u_voxelTex",
                                                                           UniformList::FindUniform("u_voxelTex",
                                                                                                    voxelMat->GetUniforms(RenderPasses::BaseComponents).TextureUniforms).Loc);
    Deadzone * deadzone = (Deadzone*)(new EmptyDeadzone());//HorizontalCrossDeadzone(Interval(0.05f, 0.1f, 0.001f)));
    Vector2Input * mouseInput = (Vector2Input*)(new MouseDeltaVector2Input(Vector2f(15.0f, 15.0f), DeadzonePtr(deadzone), sf::Vector2i(100, 100),
                                                                           Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y)));
    player.Cam = VoxelCamera(Vector3f(2.0f, 2.0f, 20.0f), LookRotation(Vector2InputPtr(mouseInput), Vector3f(0.0f, 2.25f, 2.65f)));
    player.Cam.Window = GetWindow();
    player.Cam.Info.SetFOVDegrees(60.0f);
    player.Cam.Info.Width = vWindowSize.x;
    player.Cam.Info.Height = vWindowSize.y;
    player.Cam.Info.zNear = 0.1f;
    player.Cam.Info.zFar = 1000.0f;


    player.CamOffset = Vector3f(0.0f, 0.0f, 0.5f);
    player.Movement = Vector2InputPtr((Vector2Input*)new FourButtonVector2Input(BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Key::D)),
                                                                                BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Key::A)),
                                                                                BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Key::W)),
                                                                                BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Key::S))));
    player.Jump = BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Key::Space, BoolInput::ValueStates::JustPressed));
}
void VoxelWorld::OnWorldEnd(void)
{
    if (voxelMat != 0) delete voxelMat;
    for (auto element = chunkMeshes.begin(); element != chunkMeshes.end(); ++element)
        delete element->second;
}

void VoxelWorld::OnWindowResized(unsigned int w, unsigned int h)
{
    glViewport(0, 0, w, h);
    vWindowSize.x = w;
    vWindowSize.y = h;
    player.Cam.Info.Width = w;
    player.Cam.Info.Height = h;
}

void VoxelWorld::UpdateWorld(float elapsed)
{
    MouseDeltaVector2Input * ptr = (MouseDeltaVector2Input*)player.Cam.RotationInput.Input.get();
    if (capMouse)
    {
        GetWindow()->setMouseCursorVisible(false);
        ptr->MouseResetPos = GetWindow()->getPosition() + sf::Vector2i(vWindowSize.x / 2, vWindowSize.y / 2);
        ptr->UseHorizontal = true;
        ptr->UseVertical = true;
    }
    else
    {
        GetWindow()->setMouseCursorVisible(true);
        ptr->MouseResetPos = sf::Vector2i(-1, -1);
        ptr->UseHorizontal = false;
        ptr->UseVertical = false;
    }


    player.Update(elapsed, GetTotalElapsedSeconds());

    if (Input.GetBoolInputValue(INPUT_Quit))
        EndWorld();
    if (Input.GetBoolInputValue(INPUT_MouseCap))
        capMouse = !capMouse;
}

void VoxelWorld::RenderOpenGL(float elapsed)
{
    renderState.EnableState();
    if (manager.CastRay(player.Cam.GetPosition(), player.Cam.GetForward(), 50.0f).Chunk == 0)
        ScreenClearer(true, true, false, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)).ClearScreen();
    else ScreenClearer(true, true, false, Vector4f(0.5f, 0.0f, 0.0f, 0.0f)).ClearScreen();

    std::vector<const Mesh*> meshes;
    meshes.insert(meshes.end(), &voxelMesh);

    TransformObject dummy;

    Matrix4f worldM, viewM, projM;
    worldM.SetAsIdentity();
    player.Cam.GetViewTransform(viewM);
    projM.SetAsPerspProj(player.Cam.Info);
    RenderInfo info(this, &player.Cam, &dummy, &worldM, &viewM, &projM);

    if (!voxelMat->Render(RenderPasses::BaseComponents, info, meshes))
    {
        PrintError("Error rendering voxel material", voxelMat->GetErrorMsg());
        EndWorld();
    }
}