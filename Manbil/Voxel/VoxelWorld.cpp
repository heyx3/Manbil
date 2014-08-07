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
#include "../Rendering/GUI/TextRenderer.h"
#include "../DebugAssist.h"


//Debug printing stuff.
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



Vector2i vWindowSize(800, 600);
const unsigned int INPUT_AddVoxel = 753321,
                   INPUT_RemoveVoxel = 123357,
                   INPUT_Quit = 6666666,
                   INPUT_MouseCap = 1337;
bool capMouse = true;


VoxelWorld::VoxelWorld(void)
    : SFMLOpenGLWorld(vWindowSize.x, vWindowSize.y, sf::ContextSettings(8, 0, 0, 4, 1)),
        voxelMat(0), voxelHighlightMat(0), voxelHighlightMesh(PrimitiveTypes::TriangleList),
        renderState(RenderingState::Cullables::C_BACK),
        player(manager), oculusDev(0), postProcessing(0),
        voxelTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PS_32F, true),
        worldRenderTargetColorTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F, false),
        worldRenderTargetDepthTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F_DEPTH, false)
{

}
VoxelWorld::~VoxelWorld(void)
{
    DeleteAndSetToNull(postProcessing);
    DeleteAndSetToNull(finalWorldRenderMat);
    DeleteAndSetToNull(finalWorldRenderQuad);
    DeleteAndSetToNull(voxelHighlightMat);
    voxelTex.DeleteIfValid();
    worldRenderTargetColorTex.DeleteIfValid();
    worldRenderTargetDepthTex.DeleteIfValid();
    assert(oculusDev == 0);
}

void VoxelWorld::SetUpVoxels(void)
{
    //Width/height/depth of the world in chunks.
    const Vector3i worldLength(5, 5, 5);

    //Create the chunks.
    Vector3i loc;
    for (loc.z = 0; loc.z < worldLength.z; ++loc.z)
        for (loc.y = 0; loc.y < worldLength.y; ++loc.y)
            for (loc.x = 0; loc.x < worldLength.x; ++loc.x)
                GetCreateChunk(loc);


    //Generate 3D noise to be converted to voxels.

    Noise3D noise(VoxelChunk::ChunkSize * worldLength.x, VoxelChunk::ChunkSize * worldLength.y, VoxelChunk::ChunkSize * worldLength.z, 0.0f);
    
#pragma warning(disable: 4127)
    if (false)
    {
        FlatNoise3D flat(1.0f);
        flat.Generate(noise);

        noise[Vector3u()] = 0.0f;
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
    else if (true)
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
#pragma warning(default: 4127)


    //Generate voxels from noise.
    for (auto location = manager.GetAllChunks().begin(); location != manager.GetAllChunks().end(); ++location)
    {
        location->second->DoToEveryVoxel([&noise, &location](Vector3u localIndex)
        {
            Vector3i noiseIndex = (location->first * VoxelChunk::ChunkSize) + Vector3i(localIndex.x, localIndex.y, localIndex.z);
            assert(noiseIndex.x >= 0 && noiseIndex.y >= 0 && noiseIndex.z >= 0);
            location->second->SetVoxelLocal(localIndex, noise[noiseIndex.CastToUInt()] > 0.5f);
        });
    }
}

void VoxelWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    InitializeStaticSystems(true, true, true);

    DataNode::ClearMaterialData();


    //Input.
    Input.AddBoolInput(INPUT_RemoveVoxel, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Left, BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_AddVoxel, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Right, BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_Quit, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Escape, BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_MouseCap, BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Space, BoolInput::ValueStates::JustPressed)));


    //Initialize the chunk mesh.
    SetUpVoxels();
    std::unordered_map<Vector3i, ChunkMesh*, Vector3i> & mshs = chunkMeshes;
    manager.DoToEveryChunk([&mshs](Vector3i chunkIndex, VoxelChunk * chnk)
    {
        mshs[chunkIndex]->RebuildMesh();
    });


    //Initialize the texture.
    voxelTex.Create();
    std::string error;
    if (!voxelTex.SetDataFromFile("Content/Textures/VoxelTex.png", error))
    {
        PrintError("Error loading voxel texture 'Content/Textures/VoxelTex.png'", error);
        EndWorld();
        return;
    }


    //Initialize post-processing.
    postProcessing = new VoxelWorldPPC(*this);
    if (postProcessing->HasError())
    {
        PrintError("Error setting up voxel world post processing chains", postProcessing->GetError());
        EndWorld();
        return;
    }

    typedef DataNode::Ptr DNP;

    //Initialize the voxel highlight.
    std::vector<VertexPosTex1Normal> vhvs;
    std::vector<unsigned int> vsis;
    PrimitiveGenerator::GenerateCube(vhvs, vsis, false, false, Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f), Vector3f(-1, -1, -1) * 0.25f, Vector3f(1, 1, 1) * 0.25f);
    RenderObjHandle vhvbo, vhibo;
    RenderDataHandler::CreateVertexBuffer(vhvbo, vhvs.data(), vhvs.size());
    RenderDataHandler::CreateIndexBuffer(vhibo, vsis.data(), vsis.size());
    voxelHighlightMesh.SetVertexIndexData(VertexIndexData(vhvs.size(), vhvbo, vsis.size(), vhibo));
    
    //Voxel highlight material.
    DataNode::VertexIns = VertexPosTex1Normal::GetAttributeData();
    DNP objPosToScreen(new SpaceConverterNode(DataLine(VertexInputNode::GetInstanceName()),
                                              SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                              SpaceConverterNode::DT_POSITION, "objtoScreenPos"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(objPosToScreen->GetName(), 1);
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_FinalColor", DataLine(VectorF(1.0f, 1.0f, 1.0f))));
    UniformDictionary vhvUD;
    ShaderGenerator::GeneratedMaterial genVHM = ShaderGenerator::GenerateMaterial(vhvUD, RenderingModes::RM_Opaque);
    if (!genVHM.ErrorMessage.empty())
    {
        PrintError("Error generating voxel highlight mesh", genVHM.ErrorMessage);
        EndWorld();
        return;
    }
    voxelHighlightMat = genVHM.Mat;


    //Initialize the final world render.

    worldRenderTarget = (*RenderTargets).CreateRenderTarget(PixelSizes::PS_32F_DEPTH);
    if (worldRenderTarget == RenderTargetManager::ERROR_ID)
    {
        PrintError("Error creating world render target", (*RenderTargets).GetError());
        EndWorld();
        return;
    }
    worldRenderTargetColorTex.Create();
    worldRenderTargetColorTex.ClearData((unsigned int)vWindowSize.x, (unsigned int)vWindowSize.y);
    worldRenderTargetDepthTex.Create();
    (*RenderTargets)[worldRenderTarget]->SetDepthAttachment(RenderTargetTex(&worldRenderTargetDepthTex));
    (*RenderTargets)[worldRenderTarget]->SetColorAttachment(RenderTargetTex(&worldRenderTargetColorTex), true);

    finalWorldRenderQuad = new DrawingQuad();

    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetAttributeData();

    //Vertex shader.
    std::vector<DataLine> toCombine;
    toCombine.insert(toCombine.end(), DataLine(VertexInputNode::GetInstanceName()));
    toCombine.insert(toCombine.end(), DataLine(1.0f));
    DNP vertPosOut(new CombineVectorNode(toCombine, "vertPosOut"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(vertPosOut->GetName());

    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_pos", DataLine(VertexInputNode::GetInstanceName(), 1)));
    //Fragment shader.
    DNP worldRenderTexPtr(new TextureSample2DNode(DataLine(FragmentInputNode::GetInstanceName(), 0),
                                               "u_finalWorldRender", "worldRenderTex"));
    DataLine worldRenderTex(worldRenderTexPtr->GetName(), TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_FinalColor", worldRenderTex));
    //Material generation.
    UniformDictionary dict;
    ShaderGenerator::GeneratedMaterial fGenM = ShaderGenerator::GenerateMaterial(dict, RenderingModes::RM_Opaque);
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
    finalWorldRenderParams.Texture2DUniforms["u_finalWorldRender"].Texture = postProcessing->GetFinalRender()->GetColorTextures()[0].MTex->GetTextureHandle();


    #pragma region Voxel material

    //Definition.
    DataNode::ClearMaterialData();
    DataNode::VertexIns = VoxelVertex::GetAttributeData();
    /* Vertex outputs:
    * 0: Min Existing
    * 1: Max Existing
    */
    DNP worldPos(new SpaceConverterNode(DataLine(VertexInputNode::GetInstanceName()),
                                        SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_WORLD,
                                        SpaceConverterNode::DT_POSITION, "worldPos"));
    DNP worldPosHomg(new CombineVectorNode(DataLine(worldPos->GetName()), DataLine(1.0f), "worldPosHomogenous"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(worldPosHomg->GetName());
    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_MinExists", DataLine(VertexInputNode::GetInstanceName(), 1)));
    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_MaxExists", DataLine(VertexInputNode::GetInstanceName(), 2)));
    /* Geometry outputs:
    * 0: World pos
    * 1: UV
    */
    voxelParams.FloatUniforms["u_corner1"] = UniformValueF(Vector3f(-1.0f, -1.0f, -1.0f), "u_corner1");
    voxelParams.FloatUniforms["u_corner2"] = UniformValueF(Vector3f(1.0f, -1.0f, -1.0f), "u_corner2");
    voxelParams.FloatUniforms["u_corner3"] = UniformValueF(Vector3f(-1.0f, 1.0f, 1.0f), "u_corner3");
    voxelParams.FloatUniforms["u_corner4"] = UniformValueF(Vector3f(1.0f, 1.0f, 1.0f), "u_corner4");
    //Use a subroutine for the function that determines how to check whether to draw the face.
    //It returns the single value from the vertex inputs that decides whether to draw.
    std::vector<std::string> subroutines;
    subroutines.insert(subroutines.end(), "sub_getQuadDecider_minX");
    subroutines.insert(subroutines.end(), "sub_getQuadDecider_minY");
    subroutines.insert(subroutines.end(), "sub_getQuadDecider_minZ");
    subroutines.insert(subroutines.end(), "sub_getQuadDecider_maxX");
    subroutines.insert(subroutines.end(), "sub_getQuadDecider_maxY");
    subroutines.insert(subroutines.end(), "sub_getQuadDecider_maxZ");
    std::shared_ptr<SubroutineDefinition> subDef(new SubroutineDefinition(ShaderHandler::Shaders::SH_GeometryShader, 1, "subroutine_getQuadDecider", std::vector<SubroutineDefinition::Parameter>()));
    voxelParams.SubroutineUniforms["u_getQuadDecider"] = UniformSubroutineValue(subDef, subroutines, 0, "u_getQuadDecider");

    MaterialUsageFlags gsFlags;
    gsFlags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_VIEWPROJ_MAT);
    std::string worldToScreen = MaterialConstants::ViewProjMatName + " * vec4(gsOut_worldPos, 1.0)";
    DataNode::GeometryShader = GeoShaderData(ShaderInOutAttributes(3, 2, false, false, "gsOut_worldPos", "gsOut_uv"),
                                             gsFlags, 4, Points, TriangleStrip, voxelParams,
                                             std::string() +
"subroutine(subroutine_getQuadDecider)                                                         \n\
float sub_getQuadDecider_minX() { return vOut_MinExists[0].x; } \n\
subroutine(subroutine_getQuadDecider)                                                          \n\
float sub_getQuadDecider_minY() { return vOut_MinExists[0].y; } \n\
subroutine(subroutine_getQuadDecider)                                                          \n\
float sub_getQuadDecider_minZ() { return vOut_MinExists[0].z; } \n\
subroutine(subroutine_getQuadDecider)                                                          \n\
float sub_getQuadDecider_maxX() { return vOut_MaxExists[0].x; } \n\
subroutine(subroutine_getQuadDecider)                                                          \n\
float sub_getQuadDecider_maxY() { return vOut_MaxExists[0].y; } \n\
subroutine(subroutine_getQuadDecider)                                                          \n\
float sub_getQuadDecider_maxZ() { return vOut_MaxExists[0].z; } \n\
                                                                                               \n\
void main()                                                                                    \n\
{                                                                                              \n\
    vec3 pos = gl_in[0].gl_Position.xyz;                                                       \n\
                                                                                               \n\
    if (u_getQuadDecider() == 1.0f)                                                            \n\
    {                                                                                          \n\
        gsOut_worldPos = pos + u_corner1;                                                      \n\
        gsOut_uv = vec2(0.0, 0.0);                                                             \n\
        gl_Position = " + worldToScreen + ";                                                   \n\
        EmitVertex();                                                                          \n\
                                                                                               \n\
        gsOut_worldPos = pos + u_corner2;                                                      \n\
        gsOut_uv = vec2(1.0, 0.0);                                                             \n\
        gl_Position = " + worldToScreen + ";                                                   \n\
        EmitVertex();                                                                          \n\
                                                                                               \n\
        gsOut_worldPos = pos + u_corner3;                                                      \n\
        gsOut_uv = vec2(0.0, 1.0);                                                             \n\
        gl_Position = " + worldToScreen + ";                                                   \n\
        EmitVertex();                                                                          \n\
                                                                                               \n\
        gsOut_worldPos = pos + u_corner4;                                                      \n\
        gsOut_uv = vec2(1.0, 1.0);                                                             \n\
        gl_Position = " + worldToScreen + ";                                                   \n\
        EmitVertex();                                                                          \n\
    }                                                                                          \n\
}");
    //Fragment shader.
    DNP surfaceNormalParam(new ParamNode(3, "u_surfaceNormal", "surfaceNormalParam"));
    DNP lightCalc(new LightingNode(DataLine(FragmentInputNode::GetInstanceName()),
                                   DataLine(surfaceNormalParam->GetName()),
                                   DataLine(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()), "lightBrightness",
                                   DataLine(0.25f), DataLine(0.75f), DataLine(3.0f), DataLine(64.0f)));
    DNP diffuseTexPtr(new TextureSample2DNode(DataLine(FragmentInputNode::GetInstanceName(), 1),
                                              "u_voxelTex", "diffuseTexSampler"));
    DataLine diffuseTex(diffuseTexPtr->GetName(), TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
    DNP finalColor(new MultiplyNode(DataLine(lightCalc->GetName()), diffuseTex));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_FinalColor", DataLine(finalColor->GetName())));

    //Generation.
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(voxelParams, RenderingModes::RM_Opaque);
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

    //Parameters.
    voxelParams.Texture2DUniforms["u_voxelTex"] = UniformSampler2DValue(voxelTex.GetTextureHandle(), "u_voxelTex",
                                                                        UniformList::FindUniform("u_voxelTex",
                                                                                                 voxelMat->GetUniforms().Texture2DUniforms).Loc);

    #pragma endregion


    //Player camera/input.
    Deadzone * deadzone = (Deadzone*)(new EmptyDeadzone());
    Vector2Input * mouseInput = (Vector2Input*)(new MouseDeltaVector2Input(Vector2f(0.35f, 0.35f), DeadzonePtr(deadzone), sf::Vector2i(100, 100),
                                                                           Vector2f((float)sf::Mouse::getPosition().x, (float)sf::Mouse::getPosition().y)));
    oculusDev = new OculusDevice(0);
    player.Cam = VoxelCamera(Vector3f(0, 0, 0),
                             LookRotation(Vector2InputPtr(mouseInput), Vector3f(0.0f, 2.25f, 2.65f)),
                             oculusDev,
                             Vector3f(1, 1, 1).Normalized());
    player.Cam.Window = GetWindow();
    player.Cam.Info.SetFOVDegrees(60.0f);
    player.Cam.Info.Width = (float)vWindowSize.x;
    player.Cam.Info.Height = (float)vWindowSize.y;
    player.Cam.Info.zNear = 0.1f;
    player.Cam.Info.zFar = 500.0f;

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

    DeleteAndSetToNull(oculusDev);
    voxelTex.DeleteIfValid();
    worldRenderTargetColorTex.DeleteIfValid();
    worldRenderTargetDepthTex.DeleteIfValid();
    DestroyStaticSystems(true, true, true);
}

void VoxelWorld::OnWindowResized(unsigned int w, unsigned int h)
{
    glViewport(0, 0, w, h);
    vWindowSize.x = w;
    vWindowSize.y = h;
    player.Cam.Info.Width = (float)w;
    player.Cam.Info.Height = (float)h;
    worldRenderTargetColorTex.ClearData(w, h);
    (*RenderTargets)[worldRenderTarget]->UpdateSize();
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
                    //Rebuild any chunks that the voxel was bordering on.
                    if (toAdd.LocalIndex.x == 0)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).LessX();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                            found->second->RebuildMesh(true);
                    }
                    else if (toAdd.LocalIndex.x == VoxelChunk::ChunkSize - 1)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).MoreX();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                            found->second->RebuildMesh(true);
                    }
                    if (toAdd.LocalIndex.y == 0)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).LessY();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                            found->second->RebuildMesh(true);
                    }
                    else if (toAdd.LocalIndex.y == VoxelChunk::ChunkSize - 1)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).MoreY();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                            found->second->RebuildMesh(true);
                    }
                    if (toAdd.LocalIndex.z == 0)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).LessZ();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                            found->second->RebuildMesh(true);
                    }
                    else if (toAdd.LocalIndex.z == VoxelChunk::ChunkSize - 1)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).MoreZ();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                            found->second->RebuildMesh(true);
                    }

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
                //Rebuild any chunks that the voxel was bordering on.
                if (hit.ChunkRayCastResult.VoxelIndex.x == 0)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).LessX();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                        found->second->RebuildMesh(true);
                }
                else if (hit.ChunkRayCastResult.VoxelIndex.x == VoxelChunk::ChunkSize - 1)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).MoreX();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                        found->second->RebuildMesh(true);
                }
                if (hit.ChunkRayCastResult.VoxelIndex.y == 0)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).LessY();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                        found->second->RebuildMesh(true);
                }
                else if (hit.ChunkRayCastResult.VoxelIndex.y == VoxelChunk::ChunkSize - 1)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).MoreY();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                        found->second->RebuildMesh(true);
                }
                if (hit.ChunkRayCastResult.VoxelIndex.z == 0)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).LessZ();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                        found->second->RebuildMesh(true);
                }
                else if (hit.ChunkRayCastResult.VoxelIndex.z == VoxelChunk::ChunkSize - 1)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).MoreZ();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                        found->second->RebuildMesh(true);
                }

                if (hit.Chunk->MinCorner == Vector3i())
                    std::cout << "Debug output: " << DebugAssist::STR << "\n\n\n";
            }
        }
    }
}

void VoxelWorld::RenderOpenGL(float elapsed)
{
    renderState.EnableState();
    Vector4f clearColor(0.0f, 0.0f, 0.0f, 0.0f);
    //if (player.Cam.OVRDevice.get() != 0 && player.Cam.OVRDevice->IsDoneAutoCalibration())
    //    clearColor.x = 0.0f;
    ScreenClearer(true, true, false, clearColor).ClearScreen();

    //Get the chunks to be drawn.
    //Draw each of the faces one at a time.
    std::vector<const Mesh*> lessX, lessY, lessZ, moreX, moreY, moreZ;
    Vector3i camPosIndex = manager.ToChunkIndex(player.Cam.GetPosition() + player.CamOffset);
    std::unordered_map<Vector3i, ChunkMesh*, Vector3i> & meshes = chunkMeshes;
    //TODO: Speed things up by keeping a persistent set of mesh vectors and only modify them when the camera moves into a new chunk.
    manager.DoToEveryChunk([camPosIndex, &meshes, &lessX, &lessY, &lessZ, &moreX, &moreY, &moreZ](Vector3i chunkIndex, VoxelChunk *chnk)
    {
        const Mesh * msh = &meshes[chunkIndex]->GetMesh();
        if (msh->GetVertexIndexData(0).GetVerticesCount() == 0) return;

        bool equalX = (camPosIndex.x == chunkIndex.x);
        if (equalX || camPosIndex.x < chunkIndex.x)
            lessX.insert(lessX.end(), msh);
        if (equalX || camPosIndex.x > chunkIndex.x)
            moreX.insert(moreX.end(), msh);

        bool equalY = (camPosIndex.y == chunkIndex.y);
        if (equalY || camPosIndex.y < chunkIndex.y)
            lessY.insert(lessY.end(), msh);
        if (equalY || camPosIndex.y > chunkIndex.y)
            moreY.insert(moreY.end(), msh);

        bool equalZ = (camPosIndex.z == chunkIndex.z);
        if (equalZ || camPosIndex.z < chunkIndex.z)
            lessZ.insert(lessZ.end(), msh);
        if (equalZ || camPosIndex.z > chunkIndex.z)
            moreZ.insert(moreZ.end(), msh);
    });

    TransformObject dummy;

    Matrix4f worldM, viewM, projM;
    worldM.SetAsIdentity();
    player.Cam.GetViewTransform(viewM);
    projM.SetAsPerspProj(player.Cam.Info);
    RenderInfo info(this, &player.Cam, &dummy, &worldM, &viewM, &projM);

    //Render the world.
    (*RenderTargets)[worldRenderTarget]->EnableDrawingInto();
    renderState.EnableState();
    ScreenClearer(true, true, false, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)).ClearScreen();


    #pragma region Render each face

    const float halfVox = 0.5f * VoxelChunk::VoxelSizeF;

    voxelParams.FloatUniforms["u_surfaceNormal"].SetValue(Vector3f(-1.0f, 0.0f, 0.0f));
    voxelParams.FloatUniforms["u_corner1"].SetValue(Vector3f(-halfVox, -halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner2"].SetValue(Vector3f(-halfVox, halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner3"].SetValue(Vector3f(-halfVox, -halfVox, halfVox));
    voxelParams.FloatUniforms["u_corner4"].SetValue(Vector3f(-halfVox, halfVox, halfVox));
    voxelParams.SubroutineUniforms["u_getQuadDecider"].ValueIndex = 0;
    if (!voxelMat->Render(info, lessX, voxelParams))
    {
        PrintError("Error rendering voxel material for \"less X\" face", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }

    voxelParams.FloatUniforms["u_surfaceNormal"].SetValue(Vector3f(0.0f, -1.0f, 0.0f));
    voxelParams.FloatUniforms["u_corner1"].SetValue(Vector3f(-halfVox, -halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner2"].SetValue(Vector3f(-halfVox, -halfVox, halfVox));
    voxelParams.FloatUniforms["u_corner3"].SetValue(Vector3f(halfVox, -halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner4"].SetValue(Vector3f(halfVox, -halfVox, halfVox));
    voxelParams.SubroutineUniforms["u_getQuadDecider"].ValueIndex = 1;
    if (!voxelMat->Render(info, lessY, voxelParams))
    {
        PrintError("Error rendering voxel material for \"less Y\" face", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }

    voxelParams.FloatUniforms["u_surfaceNormal"].SetValue(Vector3f(0.0f, 0.0f, -1.0f));
    voxelParams.FloatUniforms["u_corner1"].SetValue(Vector3f(-halfVox, -halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner2"].SetValue(Vector3f(halfVox, -halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner3"].SetValue(Vector3f(-halfVox, halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner4"].SetValue(Vector3f(halfVox, halfVox, -halfVox));
    voxelParams.SubroutineUniforms["u_getQuadDecider"].ValueIndex = 2;
    if (!voxelMat->Render(info, lessZ, voxelParams))
    {
        PrintError("Error rendering voxel material for \"less Z\" face", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }

    voxelParams.FloatUniforms["u_surfaceNormal"].SetValue(Vector3f(1.0f, 0.0f, 0.0f));
    voxelParams.FloatUniforms["u_corner1"].SetValue(Vector3f(halfVox, -halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner2"].SetValue(Vector3f(halfVox, -halfVox, halfVox));
    voxelParams.FloatUniforms["u_corner3"].SetValue(Vector3f(halfVox, halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner4"].SetValue(Vector3f(halfVox, halfVox, halfVox));
    voxelParams.SubroutineUniforms["u_getQuadDecider"].ValueIndex = 3;
    if (!voxelMat->Render(info, moreX, voxelParams))
    {
        PrintError("Error rendering voxel material for \"more X\" face", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }

    voxelParams.FloatUniforms["u_surfaceNormal"].SetValue(Vector3f(0.0f, 1.0f, 0.0f));
    voxelParams.FloatUniforms["u_corner1"].SetValue(Vector3f(-halfVox, halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner2"].SetValue(Vector3f(halfVox, halfVox, -halfVox));
    voxelParams.FloatUniforms["u_corner3"].SetValue(Vector3f(-halfVox, halfVox, halfVox));
    voxelParams.FloatUniforms["u_corner4"].SetValue(Vector3f(halfVox, halfVox, halfVox));
    voxelParams.SubroutineUniforms["u_getQuadDecider"].ValueIndex = 4;
    if (!voxelMat->Render(info, moreY, voxelParams))
    {
        PrintError("Error rendering voxel material for \"more Y\" face", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }

    voxelParams.FloatUniforms["u_surfaceNormal"].SetValue(Vector3f(0.0f, 0.0f, 1.0f));
    voxelParams.FloatUniforms["u_corner1"].SetValue(Vector3f(-halfVox, -halfVox, halfVox));
    voxelParams.FloatUniforms["u_corner2"].SetValue(Vector3f(-halfVox, halfVox, halfVox));
    voxelParams.FloatUniforms["u_corner3"].SetValue(Vector3f(halfVox, -halfVox, halfVox));
    voxelParams.FloatUniforms["u_corner4"].SetValue(Vector3f(halfVox, halfVox, halfVox));
    voxelParams.SubroutineUniforms["u_getQuadDecider"].ValueIndex = 5;
    if (!voxelMat->Render(info, moreZ, voxelParams))
    {
        PrintError("Error rendering voxel material for \"more Z\" face", voxelMat->GetErrorMsg());
        EndWorld();
        return;
    }

    #pragma endregion


    std::vector<const Mesh*> highlightMsh;
    highlightMsh.insert(highlightMsh.end(), &voxelHighlightMesh);
    if (!voxelHighlightMat->Render(info, highlightMsh, voxelHighlightParams))
    {
        PrintError("Error rendering voxel highlight", voxelHighlightMat->GetErrorMsg());
        EndWorld();
        return;
    }
    (*RenderTargets)[worldRenderTarget]->DisableDrawingInto(vWindowSize.x, vWindowSize.y, true);


    //Render the post-process chain.
    if (!postProcessing->RenderPostProcessing((*RenderTargets)[worldRenderTarget]->GetColorTextures()[0].MTex->GetTextureHandle(),
                                              (*RenderTargets)[worldRenderTarget]->GetDepthTexture().MTex->GetTextureHandle(),
                                              player.Cam.Info))
    {
        PrintError("Error rendering post-process chains", postProcessing->GetError());
        EndWorld();
        return;
    }
    
    //Render the final world info.
    finalWorldRenderParams.Texture2DUniforms["u_finalWorldRender"].Texture = postProcessing->GetFinalRender()->GetColorTextures()[0].MTex->GetTextureHandle();
    ScreenClearer().ClearScreen();
    if (!finalWorldRenderQuad->Render(info, finalWorldRenderParams, *finalWorldRenderMat))
    {
        PrintError("Error rendering final world render", finalWorldRenderMat->GetErrorMsg());
        EndWorld();
        return;
    }
}