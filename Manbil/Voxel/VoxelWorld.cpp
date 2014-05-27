#include "VoxelWorld.h"

#include "../Math/Higher Math/GeometricMath.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../ScreenClearer.h"
#include "../Math/Shapes/ThreeDShapes.h"
#include "../Input/Input.hpp"
#include "../Math/NoiseGeneration.hpp"
#include "../Rendering/PrimitiveGenerator.h"
#include "VoxelWorldPPC.h"
#include "../DebugAssist.h"


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
const unsigned int INPUT_AddVoxel = 753321,
                   INPUT_RemoveVoxel = 123357,
                   INPUT_Quit = 6666666,
                   INPUT_MouseCap = 1337;
bool capMouse = true;


VoxelWorld::VoxelWorld(void)
    : SFMLOpenGLWorld(vWindowSize.x, vWindowSize.y, sf::ContextSettings(8, 0, 0, 3, 1)),
        voxelMat(0),
        renderState(RenderingState::Cullables::C_NONE),
        voxelMesh(PrimitiveTypes::TriangleList),
        player(manager), oculusDev(0), postProcessing(0),
        voxelHighlightMat(0),
        voxelHighlightMesh(PrimitiveTypes::TriangleList)
{
}
VoxelWorld::~VoxelWorld(void)
{
    DeleteAndSetToNull(postProcessing);
    DeleteAndSetToNull(finalWorldRenderMat);
    DeleteAndSetToNull(finalWorldRenderQuad);
    DeleteAndSetToNull(voxelHighlightMat);
    assert(oculusDev == 0);
}

void VoxelWorld::SetUpVoxels(void)
{
    //Width/height/depth of the world in chunks.
    const Vector3i worldLength(2, 2, 2);

    //Create the chunks.
    Vector3i loc;
    for (loc.z = 0; loc.z < worldLength.z; ++loc.z)
        for (loc.y = 0; loc.y < worldLength.y; ++loc.y)
            for (loc.x = 0; loc.x < worldLength.x; ++loc.x)
                GetCreateChunk(loc);


    //Generate 3D noise to be converted to voxels.

    Noise3D noise(VoxelChunk::ChunkSize * worldLength.x, VoxelChunk::ChunkSize * worldLength.y, VoxelChunk::ChunkSize * worldLength.z, 0.0f);
    
    if (true)
    {
        FlatNoise3D flat(1.0f);
        flat.Generate(noise);

        //PRIORITY: Notice that using the following line changes how the bug manifests itself. This indicates a problem with RE-calculating a mesh.
        noise[Vector3i()] = 0.0f;
    }
    else if (false)
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
    else if (false)
    {
        Perlin3D perl(Vector3f(20.0f, 20.0f, 20.0f), Perlin3D::Smoothness::Linear, Vector3i(), 12654);
        perl.Generate(noise);

        NoiseFilterer3D nf3;
        MaxFilterVolume mfv;
        nf3.FillVolume = &mfv;

        nf3.Increase_Amount = 0.0f;
        nf3.Increase(&noise);
    }
    else if (false)
    {
        Worley3D wor(12345, 30, 5, 15);
        wor.DistFunc = &Worley3D::StraightLineDistance;
        wor.ValueGenerator = [](Worley3D::DistanceValues vals) { return -vals.Values[2] + vals.Values[0]; };
        wor.Generate(noise);

        NoiseFilterer3D nf3;
        MaxFilterVolume mfv;
        nf3.FillVolume = &mfv;

        nf3.Increase_Amount = -0.25f;
        nf3.Increase(&noise);
    }
    else if (false)
    {
        NoiseFilterer3D nf3;
        MaxFilterVolume mfv;
        CubeFilterVolume cfv(Vector3i(), Vector3i(12, 12, 12));
        nf3.FillVolume = &cfv;
        //noise.Fill(1.0f);
        nf3.Set_Value = 1.0f;
        nf3.Set(&noise);
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

    OculusDevice::InitializeSystem();

    std::unordered_map<RenderingChannels, DataLine> channels;


    //Input.
    Input.AddBoolInput(INPUT_RemoveVoxel, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Left, BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_AddVoxel, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Right, BoolInput::ValueStates::JustPressed)));
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

    voxelParams.FloatUniforms["u_castPos"].SetValue(Vector3f(0.0f, 0.0f, 0.0f));


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


    //Initialize the voxel highlight.
    std::vector<VertexPosTex1Normal> vhvs;
    std::vector<unsigned int> vsis;
    PrimitiveGenerator::GenerateCube(vhvs, vsis, false, Vector3f(-1, -1, -1) * 0.25f, Vector3f(1, 1, 1) * 0.25f);
    RenderObjHandle vhvbo, vhibo;
    RenderDataHandler::CreateVertexBuffer(vhvbo, vhvs.data(), vhvs.size());
    RenderDataHandler::CreateIndexBuffer(vhibo, vsis.data(), vsis.size());
    voxelHighlightMesh.SetVertexIndexData(VertexIndexData(vhvs.size(), vhvbo, vsis.size(), vhibo));
    channels[RenderingChannels::RC_ScreenVertexPosition] = DataNodeGenerators::ObjectPosToScreenPos<VertexPosTex1Normal>(0);
    channels[RenderingChannels::RC_VERTEX_OUT_1] = DataLine(DataNodePtr(new VertexInputNode(VertexPosTex1Normal::GetAttributeData())), 1);
    channels[RenderingChannels::RC_Color] = DataLine(Vector3f(1.0f, 1.0f, 1.0f));
    UniformDictionary vhvUD;
    ShaderGenerator::GeneratedMaterial genVHM = ShaderGenerator::GenerateMaterial(channels, vhvUD, VertexPosTex1Normal::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
    if (!genVHM.ErrorMessage.empty())
    {
        PrintError("Error generating voxel highlight mesh", genVHM.ErrorMessage);
        EndWorld();
        return;
    }
    voxelHighlightMat = genVHM.Mat;


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
    channels.clear();

    std::vector<DataLine> toCombine;
    toCombine.insert(toCombine.end(), DataLine(DataNodePtr(new VertexInputNode(DrawingQuad::GetAttributeData())), 0));
    toCombine.insert(toCombine.end(), DataLine(1.0f));
    channels[RenderingChannels::RC_ScreenVertexPosition] = DataLine(DataNodePtr(new CombineVectorNode(toCombine)), 0);

    channels[RenderingChannels::RC_VERTEX_OUT_1] = DataLine(DataNodePtr(new VertexInputNode(DrawingQuad::GetAttributeData())), 1);

    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new TextureSampleNode(DataLine(DataNodePtr(new VertexOutputNode(RenderingChannels::RC_VERTEX_OUT_1, 2)), 0),
                                                                                       "u_finalWorldRender")),
                                                     TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    UniformDictionary dict;
    ShaderGenerator::GeneratedMaterial fGenM = ShaderGenerator::GenerateMaterial(channels, dict, DrawingQuad::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
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
    finalWorldRenderParams.AddUniforms(dict, true);
    finalWorldRenderParams.TextureUniforms["u_finalWorldRender"].Texture.SetData(postProcessing->GetFinalRender()->GetColorTextures()[0]);


    //Initialize the voxel material.
    channels.clear();
    /* Vertex outputs:
     * 1 = world pos
     * 2 = world normal
     * 3 = UV
    */
    channels[RenderingChannels::RC_ScreenVertexPosition] = DataNodeGenerators::ObjectPosToScreenPos<VoxelVertex>(0);
    channels[RenderingChannels::RC_VERTEX_OUT_1] = DataLine(DataNodePtr(new ObjectPosToWorldPosCalcNode(DataLine(DataNodePtr(new VertexInputNode(VoxelVertex::GetAttributeData())), 0))), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_2] = DataLine(DataNodePtr(new ObjectNormalToWorldNormalCalcNode(DataLine(DataNodePtr(new VertexInputNode(VoxelVertex::GetAttributeData())), 2))), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_3] = DataLine(DataNodePtr(new VertexInputNode(VoxelVertex::GetAttributeData())), 1);
    
    DataLine lighting(DataNodePtr(new LightingNode(DataLine(DataNodePtr(new VertexOutputNode(RenderingChannels::RC_VERTEX_OUT_1, 3)), 0),
                                                   DataLine(DataNodePtr(new VertexOutputNode(RenderingChannels::RC_VERTEX_OUT_2, 3)), 0),
                                                   DataLine(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
                                                   DataLine(0.25f), DataLine(0.75f), DataLine(3.0f), DataLine(64.0f))),
                      0);
    DataLine diffTex(DataNodePtr(new TextureSampleNode(DataLine(DataNodePtr(new VertexOutputNode(RenderingChannels::RC_VERTEX_OUT_3, 2)), 0),
                                                       "u_voxelTex")),
                     TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new MultiplyNode(lighting, diffTex)), 0);
    dict.ClearUniforms();
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(channels, dict, VoxelVertex::GetAttributeData(), RenderingModes::RM_Opaque, true, LightSettings(false));
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
    std::unordered_map<std::string, UniformValueF> & fUnis = voxelParams.FloatUniforms;
    const std::vector<UniformList::Uniform> & unis = voxelMat->GetUniforms(RenderPasses::BaseComponents).FloatUniforms;
    fUnis["u_castPos"].Location = UniformList::FindUniform("u_castPos", unis).Loc;
    voxelParams.TextureUniforms["u_voxelTex"] = UniformSamplerValue(Textures[voxelTex], "u_voxelTex",
                                                                    UniformList::FindUniform("u_voxelTex",
                                                                                             voxelMat->GetUniforms(RenderPasses::BaseComponents).TextureUniforms).Loc);
    Deadzone * deadzone = (Deadzone*)(new EmptyDeadzone());
    Vector2Input * mouseInput = (Vector2Input*)(new MouseDeltaVector2Input(Vector2f(0.35f, 0.35f), DeadzonePtr(deadzone), sf::Vector2i(100, 100),
                                                                           Vector2f(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y)));
    oculusDev = new OculusDevice(0);
    player.Cam = VoxelCamera(Vector3f(0, 0, 0),
                             LookRotation(Vector2InputPtr(mouseInput), Vector3f(0.0f, 2.25f, 2.65f)),
                             oculusDev,
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


    //if (player.Cam.OVRDevice.get() != 0)
    //{
        //OculusDevice* dv = player.Cam.OVRDevice.get();
        //dv->StartAutoCalibration();
    //}
}
void VoxelWorld::OnWorldEnd(void)
{
    if (voxelMat != 0) delete voxelMat;
    for (auto element = chunkMeshes.begin(); element != chunkMeshes.end(); ++element)
        delete element->second;

    DeleteAndSetToNull(oculusDev);
    OculusDevice::DestroySystem();
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
    //See if a block was hit.
    VoxelChunkManager::RayCastResult castHit = manager.CastRay(player.Cam.GetPosition(), player.Cam.GetForward(), 100.0f);
    if (castHit.ChunkRayCastResult.CastResult.DidHitTarget)
    {
        voxelHighlightMesh.Transform.SetPosition(castHit.ChunkRayCastResult.CastResult.HitPos);
    }
    else
    {
        voxelHighlightMesh.Transform.SetPosition(Vector3f(-1, -1, -1) * 99999.0f);
    }

    //Mouse capturing.
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


    //Update player/camera.
    player.Update(elapsed, GetTotalElapsedSeconds());
    if (oculusDev->IsValid())
        oculusDev->Update();


    //Input handling.

    if (Input.GetBoolInputValue(INPUT_MouseCap))
        capMouse = !capMouse;
    if (capMouse)
    {
        if (Input.GetBoolInputValue(INPUT_Quit))
            EndWorld();
        if (Input.GetBoolInputValue(INPUT_AddVoxel))
        {
            VoxelChunkManager::RayCastResult hit = manager.CastRay(player.Cam.GetPosition(), player.Cam.GetForward());
            if (hit.ChunkRayCastResult.CastResult.DidHitTarget)
            {
                VoxelChunkManager::VoxelLocation toAdd = manager.GetOffset(VoxelChunkManager::VoxelLocation(hit.Chunk, hit.ChunkRayCastResult.VoxelIndex),
                                                                           hit.ChunkRayCastResult.Face);
                if (toAdd.Chunk != 0)
                {
                    std::cout << "Adding a voxel. Chunk world min pos: " << toAdd.Chunk->MinCorner.x << "," << toAdd.Chunk->MinCorner.y << "," << toAdd.Chunk->MinCorner.z << "\n" <<
                                 "Local voxel index: " << toAdd.LocalIndex.x << "," << toAdd.LocalIndex.y << "," << toAdd.LocalIndex.z << "\n";
                    toAdd.Chunk->SetVoxelLocal(toAdd.LocalIndex, true);
                    chunkMeshes[manager.GetChunkIndex(toAdd.Chunk)]->RebuildMesh(true);

                    if (toAdd.Chunk->MinCorner == Vector3i())
                        std::cout << "Debug output: " << DebugAssist::STR << "\n\n\n";
                }
            }
        }
        if (Input.GetBoolInputValue(INPUT_RemoveVoxel))
        {
            VoxelChunkManager::RayCastResult hit = manager.CastRay(player.Cam.GetPosition(), player.Cam.GetForward());
            if (hit.ChunkRayCastResult.CastResult.DidHitTarget)
            {
                std::cout << "Removing a voxel. Chunk world min pos: " << hit.Chunk->MinCorner.x << "," << hit.Chunk->MinCorner.y << "," << hit.Chunk->MinCorner.z << "\n" <<
                             "Local voxel index: " << hit.ChunkRayCastResult.VoxelIndex.x << "," << hit.ChunkRayCastResult.VoxelIndex.y << "," << hit.ChunkRayCastResult.VoxelIndex.z << "\n";
                hit.Chunk->SetVoxelLocal(hit.ChunkRayCastResult.VoxelIndex, false);
                chunkMeshes[manager.GetChunkIndex(hit.Chunk)]->RebuildMesh(true);

                if (hit.Chunk->MinCorner == Vector3i())
                    std::cout << "Debug output: " << DebugAssist::STR << "\n\n\n";
            }
        }
    }
}

void VoxelWorld::RenderOpenGL(float elapsed)
{
    renderState.EnableState();
    Vector4f clearColor(1.0f, 0.0f, 0.0f, 0.0f);
    //if (player.Cam.OVRDevice.get() != 0 && player.Cam.OVRDevice->IsDoneAutoCalibration())
    //    clearColor.x = 0.0f;
    ScreenClearer(true, true, false, clearColor).ClearScreen();

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
    renderState.EnableState();
    ScreenClearer().ClearScreen();
    if (!voxelMat->Render(RenderPasses::BaseComponents, info, meshes, voxelParams))
    {
        PrintError("Error rendering voxel material", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }
    meshes.clear();
    meshes.insert(meshes.end(), &voxelHighlightMesh);
    if (!voxelHighlightMat->Render(RenderPasses::BaseComponents, info, meshes, voxelHighlightParams))
    {
        PrintError("Error rendering voxel highlight", voxelHighlightMat->GetErrorMsg());
        EndWorld();
        return;
    }
    RenderTargets[worldRenderTarget]->DisableDrawingInto(vWindowSize.x, vWindowSize.y);


    //Render the post-process chain.
    //if (!postProcessing->RenderPostProcessing(RenderTargets[worldRenderTarget]->GetColorTextures()[0], RenderTargets[worldRenderTarget]->GetDepthTexture(), player.Cam.Info))
    //{
    //    PrintError("Error rendering post-process chains", postProcessing->GetError());
    //    EndWorld();
    //    return;
    //}
    
    //Render the final world info.
    finalWorldRenderParams.TextureUniforms["u_finalWorldRender"].Texture.SetData(RenderTargets[worldRenderTarget]->GetColorTextures()[0]);//postProcessing->GetFinalRender()->GetColorTextures()[0]);
    ScreenClearer().ClearScreen();
    if (!finalWorldRenderQuad->Render(RenderPasses::BaseComponents, info, finalWorldRenderParams, *finalWorldRenderMat))
    {
        PrintError("Error rendering final world render", finalWorldRenderMat->GetErrorMsg());
        EndWorld();
        return;
    }
}