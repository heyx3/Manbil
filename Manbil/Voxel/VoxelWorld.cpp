#include "VoxelWorld.h"

#include "../Math/Higher Math/GeometricMath.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../ScreenClearer.h"
#include "../Math/Shapes/ThreeDShapes.h"
#include "../Input/Input.hpp"
#include "../Math/NoiseGeneration.hpp"
#include "VoxelWorldPPC.h"


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



Vector2i vWindowSize(300, 300);
const unsigned int INPUT_AddVoxel = 123753,
                   INPUT_Quit = 6666666,
                   INPUT_MouseCap = 1337;
bool capMouse = true;


VoxelWorld::VoxelWorld(void)
    : SFMLOpenGLWorld(vWindowSize.x, vWindowSize.y, sf::ContextSettings(8, 0, 0, 3, 1)),
        voxelMat(0),
        renderState(RenderingState::Cullables::C_BACK),
        voxelMesh(PrimitiveTypes::Triangles),
        player(manager), postProcessing(0)
{
}
VoxelWorld::~VoxelWorld(void)
{
    DeleteAndSetToNull(postProcessing);
    DeleteAndSetToNull(finalWorldRenderMat);
    DeleteAndSetToNull(finalWorldRenderQuad);
}

void VoxelWorld::SetUpVoxels(void)
{
    //Width/height/depth of the world in chunks.
    const Vector3i worldLength(9, 9, 9);

    //Create the chunks.
    Vector3i loc;
    for (loc.z = 0; loc.z < worldLength.z; ++loc.z)
        for (loc.y = 0; loc.y < worldLength.y; ++loc.y)
            for (loc.x = 0; loc.x < worldLength.x; ++loc.x)
                GetCreateChunk(loc);


    //Generate 3D noise to be converted to voxels.

    Noise3D noise(VoxelChunk::ChunkSize * worldLength.x, VoxelChunk::ChunkSize * worldLength.y, VoxelChunk::ChunkSize * worldLength.z);
    
    if (false)
    {
        Perlin3D perl(Vector3f(30.0f, 30.0f, 60.0f), Perlin3D::Smoothness::Linear, Vector3i(), 12654);
        perl.Generate(noise);

        NoiseFilterer3D nf3;
        MaxFilterVolume mfv;
        nf3.FillVolume = &mfv;

        nf3.Increase_Amount = 0.2f;
        nf3.Increase(&noise);

        //TODO: New array to smooth into, instead of reading from the same array that is being smoothed into!
        //nf3.Smooth(&noise);
        //nf3.Smooth(&noise);

        //Add a floor.
        const int floorHeight = 10;
        CubeFilterVolume cfv(Vector3i(), Vector3i(worldLength.x * VoxelChunk::ChunkSize, worldLength.y * VoxelChunk::ChunkSize, floorHeight));
        nf3.FillVolume = &cfv;
        nf3.Set_Value = 1.0f;
        nf3.Set(&noise);
    }
    else if (true)
    {
        Worley3D wor(12345, 45, Interval(8, 10));
        wor.DistFunc = &Worley3D::StraightLineDistance;
        wor.ValueGenerator = [](Worley3D::DistanceValues vals) { return -vals.Values[0]; };
        wor.Generate(noise);

        NoiseFilterer3D nf3;
        MaxFilterVolume mfv;
        nf3.FillVolume = &mfv;

        nf3.Increase_Amount = 0.2;
        nf3.Increase(&noise);
    }
    else assert(false);


    //Generate voxels from noise.
    for (auto location = manager.GetAllChunks().begin(); location != manager.GetAllChunks().end(); ++location)
    {
        location->second->DoToEveryVoxel([&noise, &location](Vector3i localIndex)
        {
            Vector3i noiseIndex = localIndex + (location->first * VoxelChunk::ChunkSize);
            location->second->SetVoxelLocal(localIndex, noise[noiseIndex] > 0.5f);
        });
    }

}

void VoxelWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    //Input.
    Input.AddBoolInput(INPUT_AddVoxel, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Left, BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_Quit, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Escape, BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_MouseCap, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Space, BoolInput::ValueStates::JustPressed)));


    //Initialize the chunk mesh.

    SetUpVoxels();

    VertexIndexData * vids = new VertexIndexData[chunkMeshes.size()];
    unsigned int index = 0;
    for (auto entry = manager.GetAllChunks().begin(); entry != manager.GetAllChunks().end(); ++entry)
    {
        assert(chunkMeshes[entry->first] != 0);
        chunkMeshes[entry->first]->RebuildMesh(true);

        vids[index] = chunkMeshes[entry->first]->GetVID();
        index += 1;
    }
    voxelMesh.SetVertexIndexData(vids, chunkMeshes.size());
    delete[] vids;

    voxelMesh.Uniforms.FloatUniforms["u_castPos"].SetValue(Vector3f(0.0f, 0.0f, 0.0f));


    //Initialize the texture.
    voxelTex = Textures.CreateTexture("Content/Textures/VoxelTex.png");
    if (voxelTex == TextureManager::UNUSED_ID)
    {
        PrintError("Error creating voxel texture 'Content/Textures/VoxelTex.png", "File not found or unable to be loaded");
        EndWorld();
        return;
    }
    Textures[voxelTex].SetData(TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP, true));


    //Initialize post-processing.
    postProcessing = new VoxelWorldPPC(*this);
    if (postProcessing->HasError())
    {
        PrintError("Error setting up voxel world post processing chains", postProcessing->GetError());
        EndWorld();
        return;
    }


    //Initialize the final world render.

    RendTargetColorTexSettings cts;
    cts.ColorAttachment = 0;
    cts.Settings.Width = vWindowSize.x;
    cts.Settings.Height = vWindowSize.y;
    cts.Settings.Size = ColorTextureSettings::CTS_32;
    cts.Settings.Settings = TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false);
    RendTargetDepthTexSettings dts;
    dts.UsesDepthTexture = true;
    dts.Settings.Size = DepthTextureSettings::DTS_24;
    dts.Settings.Settings = TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false);
    worldRenderTarget = RenderTargets.CreateRenderTarget(cts, dts);

    finalWorldRenderQuad = new DrawingQuad();
    std::unordered_map<RenderingChannels, DataLine> channels;

    std::vector<DataLine> toCombine;
    toCombine.insert(toCombine.end(), DataLine(DataNodePtr(new ObjectPosNode()), 0));
    toCombine.insert(toCombine.end(), DataLine(1.0f));
    channels[RenderingChannels::RC_ScreenVertexPosition] = DataLine(DataNodePtr(new CombineVectorNode(toCombine)), 0);

    channels[RenderingChannels::RC_VERTEX_OUT_1] = DataLine(DataNodePtr(new UVNode()), 0);

    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new TextureSampleNode(DataLine(DataNodePtr(new VertexOutputNode(RenderingChannels::RC_VERTEX_OUT_1, 2)), 0),
                                                                                       "u_finalWorldRender")),
                                                     TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    UniformDictionary dict;
    ShaderGenerator::GeneratedMaterial fGenM = ShaderGenerator::GenerateMaterial(channels, dict, RenderingModes::RM_Opaque, false, LightSettings(false));
    if (!fGenM.ErrorMessage.empty())
    {
        PrintError("Error generating shader code for final render material", fGenM.ErrorMessage);
        EndWorld();
        return;
    }
    finalWorldRenderMat = fGenM.Mat;
    if (finalWorldRenderMat->HasError())
    {
        PrintError("Error compiling final render material", finalWorldRenderMat->GetErrorMsg());
        EndWorld();
        return;
    }
    finalWorldRenderQuad->GetMesh().Uniforms.AddUniforms(dict, true);
    finalWorldRenderQuad->GetMesh().Uniforms.TextureUniforms["u_finalWorldRender"].Texture.SetData(postProcessing->GetFinalRender()->GetColorTextures()[0]);


    //Initialize the voxel material.
    channels.clear();
    //DataLine dist = DataLine(DataNodePtr(new DistanceNode(DataLine(DataNodePtr(new WorldPosNode()), 0),
    //                                                      DataLine(DataNodePtr(new ParamNode(3, "u_castPos")), 0))), 0);
    //DataLine distMultiplier = DataLine(DataNodePtr(new ClampNode(DataLine(0.1f), DataLine(1.0f),
    //                                                             DataLine(
    //                                                                DataNodePtr(
    //                                                                    new MultiplyNode(DataLine(0.25f),
    //                                                                                     DataLine(
    //                                                                                        DataNodePtr(
    //                                                                                            new SubtractNode(DataLine(10.0f), dist)), 0))), 0))), 0);
    /* Vertex outputs:
     * 1 = world pos
     * 2 = world normal
     * 3 = UV
    */
    channels[RenderingChannels::RC_VERTEX_OUT_1] = DataLine(DataNodePtr(new ObjectPosToWorldPosCalcNode()), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_2] = DataLine(DataNodePtr(new ObjectNormalToWorldNormalCalcNode()), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_3] = DataLine(DataNodePtr(new UVNode()), 0);
    
    DataLine lighting(DataNodePtr(new LightingNode(DataLine(DataNodePtr(new VertexOutputNode(RenderingChannels::RC_VERTEX_OUT_1, 3)), 0),
                                                   DataLine(DataNodePtr(new VertexOutputNode(RenderingChannels::RC_VERTEX_OUT_2, 3)), 0),
                                                   DataLine(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
                                                   DataLine(0.25f), DataLine(0.75f), DataLine(3.0f), DataLine(64.0f))),
                      0);
    DataLine diffTex(DataNodePtr(new TextureSampleNode(DataLine(DataNodePtr(new VertexOutputNode(RenderingChannels::RC_VERTEX_OUT_3, 2)), 0),
                                                       "u_voxelTex")),
                     TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new MultiplyNode(lighting, diffTex)), 0);
    //channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new TextureSampleNode("u_voxelTex")), TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    //channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new MultiplyNode(channels[RenderingChannels::RC_Color], distMultiplier)), 0);
    //channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new MaxMinNode(DataLine(DataNodePtr(new WorldNormalNode()), 0), DataLine(0.1f), true)), 0);
    dict.ClearUniforms();
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(channels, dict, RenderingModes::RM_Opaque, true, LightSettings(false));
    if (!genM.ErrorMessage.empty())
    {
        PrintError("Error generating voxel material's shaders", genM.ErrorMessage);
        EndWorld();
        return;
    }
    voxelMat = genM.Mat;
    if (voxelMat->HasError())
    {
        PrintError("Error creating voxel material", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }
    std::unordered_map<std::string, UniformValueF> & fUnis = voxelMesh.Uniforms.FloatUniforms;
    const std::vector<UniformList::Uniform> & unis = voxelMat->GetUniforms(RenderPasses::BaseComponents).FloatUniforms;
    fUnis["u_castPos"].Location = UniformList::FindUniform("u_castPos", unis).Loc;
    voxelMesh.Uniforms.TextureUniforms["u_voxelTex"] = UniformSamplerValue(Textures[voxelTex], "u_voxelTex",
                                                                           UniformList::FindUniform("u_voxelTex",
                                                                                                    voxelMat->GetUniforms(RenderPasses::BaseComponents).TextureUniforms).Loc);
    Deadzone * deadzone = (Deadzone*)(new EmptyDeadzone());
    Vector2Input * mouseInput = (Vector2Input*)(new MouseDeltaVector2Input(Vector2f(0.35f, 0.35f), DeadzonePtr(deadzone), sf::Vector2i(100, 100),
                                                                           Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y)));
    player.Cam = VoxelCamera(Vector3f(60, 60, 60), LookRotation(Vector2InputPtr(mouseInput), Vector3f(0.0f, 2.25f, 2.65f)),
                             Vector3f(1, 1, 1).Normalized());
    player.Cam.Window = GetWindow();
    player.Cam.Info.SetFOVDegrees(60.0f);
    player.Cam.Info.Width = vWindowSize.x;
    player.Cam.Info.Height = vWindowSize.y;
    player.Cam.Info.zNear = 0.1f;
    player.Cam.Info.zFar = 1000.0f;

    player.MoveSpeed = 15.0f;
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
    if (!RenderTargets.ResizeTarget(worldRenderTarget, w, h))
    {
        PrintError("Error resizing world render target", RenderTargets.GetError());
        EndWorld();
        return;
    }
    if (!postProcessing->OnWindowResized(w, h))
    {
        PrintError("Error resizing post processing chains", postProcessing->GetError());
        EndWorld();
        return;
    }
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
    //Ray-casting test.
    if (false)
    {
        VoxelChunkManager::RayCastResult cast =
            manager.CastRay(player.Cam.GetPosition(), player.Cam.GetForward(), 50.0f);
        Vector3f pos;
        if (cast.ChunkRayCastResult.CastResult.DidHitTarget)
        {
            pos = cast.ChunkRayCastResult.CastResult.HitPos;
            std::cout << "Pos: " << pos.x << ", " << pos.y << ", " << pos.z << "\n";
        }
        voxelMesh.Uniforms.FloatUniforms["u_castPos"].SetValue(pos);
    }


    renderState.EnableState();
    ScreenClearer().ClearScreen();

    std::vector<const Mesh*> meshes;
    meshes.insert(meshes.end(), &voxelMesh);

    TransformObject dummy;

    Matrix4f worldM, viewM, projM;
    worldM.SetAsIdentity();
    player.Cam.GetViewTransform(viewM);
    projM.SetAsPerspProj(player.Cam.Info);
    RenderInfo info(this, &player.Cam, &dummy, &worldM, &viewM, &projM);

    //Render the world.
    RenderTargets[worldRenderTarget]->EnableDrawingInto();
    ScreenClearer().ClearScreen();
    if (!voxelMat->Render(RenderPasses::BaseComponents, info, meshes))
    {
        PrintError("Error rendering voxel material", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }
    RenderTargets[worldRenderTarget]->DisableDrawingInto(vWindowSize.x, vWindowSize.y);


    //Render the post-process chain.
    if (!postProcessing->RenderPostProcessing(RenderTargets[worldRenderTarget]->GetColorTextures()[0], RenderTargets[worldRenderTarget]->GetDepthTexture(), player.Cam.Info))
    {
        PrintError("Error rendering post-process chains", postProcessing->GetError());
        EndWorld();
        return;
    }
    
    //Render the final world info.
    finalWorldRenderQuad->GetMesh().Uniforms.TextureUniforms["u_finalWorldRender"].Texture.SetData(postProcessing->GetFinalRender()->GetColorTextures()[0]);
    ScreenClearer().ClearScreen();
    if (!finalWorldRenderQuad->Render(RenderPasses::BaseComponents, info, *finalWorldRenderMat))
    {
        PrintError("Error rendering final world render", finalWorldRenderMat->GetErrorMsg());
        EndWorld();
        return;
    }
}