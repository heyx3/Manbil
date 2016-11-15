#include "VoxelWorld.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/Data Nodes/ShaderGenerator.h"
#include "../Rendering/Basic Rendering/ScreenClearer.h"
#include "../Math/Shapes/ThreeDShapes.h"
#include "../Input/Input.hpp"
#include "../Math/NoiseGeneration.hpp"
#include "../Rendering/Primitives/PrimitiveGenerator.h"
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
                   INPUT_MouseCap = 1337,
                   INPUT_UpFov = 112255,
                   INPUT_DownFov = 61;
bool capMouse = true;
float fov = 75.0f;


VoxelWorld::VoxelWorld(void)
    : SFMLOpenGLWorld(vWindowSize.x, vWindowSize.y),
      voxelMat(0), voxelHighlightMat(0),
      renderState(RenderingState::Cullables::C_BACK),
      player(manager), postProcessing(0), worldRenderTarget(0),
      voxelTex(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PS_32F, true),
      worldRenderTargetColorTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F, false),
      worldRenderTargetDepthTex(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PS_32F_DEPTH, false)
{

}

void VoxelWorld::SetUpVoxels(void)
{
    //Width/height/depth of the world in chunks.
    const Vector3i worldLength(5, 5, 5);

    //Create the chunks.
    Vector3i loc;
    for (loc.z = 0; loc.z < worldLength.z; ++loc.z)
    {
        for (loc.y = 0; loc.y < worldLength.y; ++loc.y)
        {
            for (loc.x = 0; loc.x < worldLength.x; ++loc.x)
            {
                GetCreateChunk(loc);
            }
        }
    }


    //Generate 3D noise to be converted to voxels.

    Noise3D noise(VoxelChunk::ChunkSize * worldLength.x,
                  VoxelChunk::ChunkSize * worldLength.y,
                  VoxelChunk::ChunkSize * worldLength.z,
                  0.0f);
    
#pragma warning(disable: 4127)
    if (false)
    {
        FlatNoise3D flat(1.0f);
        flat.Generate(noise);

        noise[Vector3u()] = 0.0f;
    }
    else if (false)
    {
        Perlin3D perl(Vector3f(30.0f, 30.0f, 60.0f), Perlin3D::Smoothness::Linear,
                      Vector3i(), 12654);
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
        CubeFilterVolume cfv(Vector3i(),
                             Vector3i(worldLength.x *
                                      VoxelChunk::ChunkSize, worldLength.y *
                                      VoxelChunk::ChunkSize, floorHeight));
        nf3.FillVolume = &cfv;
        nf3.Set_Value = 1.0f;
        nf3.Set(&noise);
    }
    else if (false)
    {
        Perlin3D perl(Vector3f(20.0f, 20.0f, 20.0f), Perlin3D::Smoothness::Linear,
                      Vector3i(), 12654);
        perl.Generate(noise);

        NoiseFilterer3D nf3;
        MaxFilterVolume mfv;
        nf3.FillVolume = &mfv;

        nf3.Increase_Amount = 0.0f;
        nf3.Increase(&noise);
    }
    else if (false)
    {
        Worley3D wor(&Worley3D::StraightLineDistance,
                     [](Worley3D::DistanceValues vals)
                     {
                         return -vals.Values[2] + vals.Values[0];
                     },
                    30);
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
        nf3.Set_Value = 1.0f;
        nf3.Set(&noise);
    }
    else if (true)
    {
        const unsigned int numbPerlins = 5;

        Perlin3D perls[numbPerlins];
        for (unsigned int i = 0; i < numbPerlins; ++i)
        {
            perls[i] = Perlin3D(64.0f * powf(0.5f, (float)i), Perlin3D::Quintic,
                                Vector3i(), Vector3i(12512, i, i * 36234).GetHashCode());
        }

        const Generator3D* gens[numbPerlins];
        for (unsigned int i = 0; i < numbPerlins; ++i)
        {
            gens[i] = &perls[i];
        }

        float weights[numbPerlins];
        for (unsigned int i = 0; i < numbPerlins; ++i)
        {
            weights[i] = powf(0.5f, (float)(i + 1));
        }

        LayeredOctave3D octvs(numbPerlins, weights, gens);
        octvs.Generate(noise);
    }
    else assert(false);
#pragma warning(default: 4127)


    //Generate voxels from noise.
    for (auto location = manager.GetAllChunks().begin();
         location != manager.GetAllChunks().end();
         ++location)
    {
        location->second->DoToEveryVoxel([&noise, &location](Vector3u localIndex)
        {
            Vector3i noiseIndex = (location->first * VoxelChunk::ChunkSize) +
                                  Vector3i(localIndex.x, localIndex.y, localIndex.z);
            location->second->SetVoxelLocal(localIndex, noise[ToV3u(noiseIndex)] > 0.5f);
        });
    }
}

void VoxelWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    DrawingQuad::InitializeQuad();


    //Input.
    Input.AddBoolInput(INPUT_RemoveVoxel,
                       BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Left,
                                                                   BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_AddVoxel,
                       BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Right,
                                                                   BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_Quit,
                       BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Escape,
                                                                      BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_MouseCap,
                       BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Space,
                                                                      BoolInput::ValueStates::JustPressed)));
    Input.AddBoolInput(INPUT_UpFov,
                       BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Up)));
    Input.AddBoolInput(INPUT_DownFov,
                       BoolInputPtr((BoolInput*)new KeyboardBoolInput(KeyboardBoolInput::Key::Down)));


    //Initialize the chunk mesh.
    SetUpVoxels();
    std::unordered_map<Vector3i, ChunkMesh*, Vector3i> & mshs = chunkMeshes;
    manager.DoToEveryChunk([&mshs](Vector3i chunkIndex, VoxelChunk* chnk)
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
    postProcessing = new VoxelWorldPPC(*this, error);
    if (!error.empty())
    {
        PrintError("Error setting up voxel world post processing chains", error);
        EndWorld();
        return;
    }

    typedef DataNode::Ptr DNP;

    //Initialize the voxel highlight.
    std::vector<VertexPosUVNormal> vhvs;
    std::vector<unsigned int> vsis;
    PrimitiveGenerator::GenerateCube(vhvs, vsis, false, false,
                                     Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f), Vector2f(1.0f, 1.0f),
                                     Vector3f(-1, -1, -1) * 0.25f, Vector3f(1, 1, 1) * 0.25f);
    voxelHighlightMesh.SubMeshes.push_back(MeshData(false, PT_TRIANGLE_LIST));
    voxelHighlightMesh.SubMeshes[0].SetVertexData(vhvs, MeshData::BUF_STATIC,
                                                  VertexPosUVNormal::GetVertexAttributes());
    voxelHighlightMesh.SubMeshes[0].SetIndexData(vsis, MeshData::BUF_STATIC);
    
    //Voxel highlight material.
    SerializedMaterial matData;
    matData.VertexInputs = VertexPosUVNormal::GetVertexAttributes();
    DNP objPosToScreen(new SpaceConverterNode(DataLine(VertexInputNode::GetInstanceName()),
                                              SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                              SpaceConverterNode::DT_POSITION, "objtoScreenPos"));
    matData.MaterialOuts.VertexPosOutput = DataLine(objPosToScreen->GetName(), 1);
    matData.MaterialOuts.FragmentOutputs.insert(matData.MaterialOuts.FragmentOutputs.end(),
                                                ShaderOutput("fOut_FinalColor",
                                                             Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));
    UniformDictionary vhvUD;
    ShaderGenerator::GeneratedMaterial genVHM =
        ShaderGenerator::GenerateMaterial(matData, vhvUD, BlendMode::GetOpaque());
    if (!genVHM.ErrorMessage.empty())
    {
        PrintError("Error generating voxel highlight mesh", genVHM.ErrorMessage);
        EndWorld();
        return;
    }
    voxelHighlightMat = genVHM.Mat;


    //Initialize the final world render.

    worldRenderTarget = new RenderTarget(PS_32F_DEPTH, error);
    if (!error.empty())
    {
        PrintError("Error creating world render target", error);
        EndWorld();
        return;
    }
    worldRenderTargetColorTex.Create();
    worldRenderTargetColorTex.ClearData((unsigned int)vWindowSize.x, (unsigned int)vWindowSize.y);
    worldRenderTargetDepthTex.Create();
    worldRenderTarget->SetDepthAttachment(RenderTargetTex(&worldRenderTargetDepthTex));
    worldRenderTarget->SetColorAttachment(RenderTargetTex(&worldRenderTargetColorTex), true);

    //Vertex shader.
    matData = SerializedMaterial();
    matData.VertexInputs = DrawingQuad::GetVertexInputData();
    std::vector<DataLine> toCombine;
    toCombine.insert(toCombine.end(), DataLine(VertexInputNode::GetInstanceName()));
    toCombine.insert(toCombine.end(), DataLine(1.0f));
    DNP vertPosOut(new CombineVectorNode(toCombine, "vertPosOut"));
    matData.MaterialOuts.VertexPosOutput = DataLine(vertPosOut->GetName());

    matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_pos",
                                                                DataLine(VertexInputNode::GetInstanceName(),
                                                                         1)));
    //Fragment shader.
    DNP worldRenderTexPtr(new TextureSample2DNode(DataLine(FragmentInputNode::GetInstanceName(), 0),
                                               "u_finalWorldRender", "worldRenderTex"));
    DataLine worldRenderTex(worldRenderTexPtr->GetName(),
                            TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    DNP finalWorldRenderCol(new CombineVectorNode(worldRenderTex, 1.0f, "finalWorldRender"));
    matData.MaterialOuts.FragmentOutputs.insert(matData.MaterialOuts.FragmentOutputs.end(),
                                                ShaderOutput("fOut_FinalColor", finalWorldRenderCol));
    //Material generation.
    UniformDictionary dict;
    ShaderGenerator::GeneratedMaterial fGenM =
        ShaderGenerator::GenerateMaterial(matData, dict, BlendMode::GetOpaque());
    if (!fGenM.ErrorMessage.empty())
    {
        PrintError("Error generating shader code for final render material", fGenM.ErrorMessage);
        EndWorld();
        return;
    }
    finalWorldRenderMat = fGenM.Mat;
    Uniform::AddUniforms(dict, finalWorldRenderParams, true);
    finalWorldRenderParams["u_finalWorldRender"].Tex() =
        postProcessing->GetFinalColor().GetTextureHandle();


    #pragma region Voxel material

    //Definition.
    matData = SerializedMaterial();
    matData.VertexInputs = VoxelVertex::GetVertexAttributes();
    /* Vertex outputs:
    * 0: Min Existing
    * 1: Max Existing
    */
    DNP worldPos(new SpaceConverterNode(DataLine(VertexInputNode::GetInstanceName()),
                                        SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_WORLD,
                                        SpaceConverterNode::DT_POSITION, "worldPos"));
    DNP worldPosHomg(new CombineVectorNode(worldPos, DataLine(1.0f), "worldPosHomogenous"));
    matData.MaterialOuts.VertexPosOutput = worldPosHomg;
    matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_MinExists",
                                                              DataLine(VertexInputNode::GetInstanceName(),
                                                                       1)));
    matData.MaterialOuts.VertexOutputs.push_back(ShaderOutput("vOut_MaxExists",
                                                              DataLine(VertexInputNode::GetInstanceName(),
                                                                       2)));
    /* Geometry outputs:
    * 0: World pos
    * 1: UV
    */
    Vector3f val(-1.0f, -1.0f, -1.0f);
    voxelParams["u_corner1"] = Uniform::MakeF("u_corner1", 3, &val.x);
    val = Vector3f(1.0f, -1.0f, -1.0f);
    voxelParams["u_corner2"] = Uniform::MakeF("u_corner2", 3, &val.x);
    val = Vector3f(-1.0f, 1.0f, 1.0f);
    voxelParams["u_corner3"] = Uniform::MakeF("u_corner3", 3, &val.x);
    val = Vector3f(1.0f, 1.0f, 1.0f);
    voxelParams["u_corner4"] = Uniform::MakeF("u_corner4", 3, &val.x);
    //Use a subroutine for the function that determines how to check whether to draw the face.
    //It returns the single value from the vertex inputs that decides whether to draw.
    std::vector<std::string> subroutines;
    subroutines.push_back("sub_getQuadDecider_minX");
    subroutines.push_back("sub_getQuadDecider_minY");
    subroutines.push_back("sub_getQuadDecider_minZ");
    subroutines.push_back("sub_getQuadDecider_maxX");
    subroutines.push_back("sub_getQuadDecider_maxY");
    subroutines.push_back("sub_getQuadDecider_maxZ");
    SubroutineDefinition subDef(SH_GEOMETRY, 1, "subroutine_getQuadDecider");
    voxelParams["u_getQuadDecider"] = Uniform("u_getQuadDecider", UT_VALUE_SUBROUTINE);
    voxelParams["u_getQuadDecider"].Subroutine() = UniformValueSubroutine(subDef, subroutines, 0);

    MaterialUsageFlags gsFlags;
    gsFlags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_VIEWPROJ_MAT);
    std::string worldToScreen = MaterialConstants::ViewProjMatName + " * vec4(gsOut_worldPos, 1.0)";
    RenderIOAttributes geoShaderAttrs(RenderIOAttributes::Attribute(3, false, "gsOut_worldPos"),
                                      RenderIOAttributes::Attribute(2, false, "gsOut_uv"));
    matData.GeoShader = GeoShaderData(geoShaderAttrs, gsFlags, 4,
                                      PT_POINTS, PT_TRIANGLE_STRIP,
                                      Uniform::MakeList(voxelParams),
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
                                   DataLine(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
                                   DataLine(0.25f), DataLine(0.75f), DataLine(3.0f), DataLine(64.0f)));
    DNP diffuseTexPtr(new TextureSample2DNode(DataLine(FragmentInputNode::GetInstanceName(), 1),
                                              "u_voxelTex", "diffuseTexSampler"));
    DataLine diffuseTex(diffuseTexPtr->GetName(), TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
    DNP finalColorRGB(new MultiplyNode(DataLine(lightCalc->GetName()), diffuseTex, "finalColorRGB"));
    DNP finalColor(new CombineVectorNode(finalColorRGB, 1.0f, "finalColor"));
    matData.MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_FinalColor", finalColor));

    //Generation.
    ShaderGenerator::GeneratedMaterial genM =
        ShaderGenerator::GenerateMaterial(matData, voxelParams, BlendMode::GetOpaque());
    if (!genM.ErrorMessage.empty())
    {
        PrintError("Error generating voxel material's shaders", genM.ErrorMessage);
        EndWorld();
        return;
    }
    voxelMat = genM.Mat;

    //Parameters.
    UniformLocation voxelTexLoc = Uniform::Find(voxelMat->GetUniforms(), "u_voxelTex")->Loc;
    voxelParams["u_voxelTex"] = Uniform("u_voxelTex", UT_VALUE_SAMPLER2D, voxelTexLoc);
    voxelParams["u_voxelTex"].Tex() = voxelTex.GetTextureHandle();

    #pragma endregion


    //Player camera/input.
    Deadzone* deadzone = (Deadzone*)(new EmptyDeadzone());
    Vector2f initialPos((float)sf::Mouse::getPosition().x,
                        (float)sf::Mouse::getPosition().y);
    Vector2Input* mouseInput = (Vector2Input*)(new MouseDeltaVector2Input(Vector2f(0.15f, 0.15f),
                                                                          DeadzonePtr(deadzone),
                                                                          sf::Vector2i(100, 100),
                                                                          initialPos));
    player.Cam = VoxelCamera(Vector3f(0, 0, 0),
                             LookRotation(Vector2InputPtr(mouseInput), Vector3f(0.0f, 2.25f, 2.65f)),
                             Vector3f(-1, -1, -1).Normalized());
    player.Cam.Window = GetWindow();
    player.Cam.PerspectiveInfo.SetFOVDegrees(fov);
    player.Cam.PerspectiveInfo.Width = (float)vWindowSize.x;
    player.Cam.PerspectiveInfo.Height = (float)vWindowSize.y;
    player.Cam.PerspectiveInfo.zNear = 0.1f;
    player.Cam.PerspectiveInfo.zFar = 500.0f;

    player.MoveSpeed = 15.0f;
    player.CamOffset = Vector3f(0.0f, 0.0f, 0.5f);
    sf::Keyboard::Key d = sf::Keyboard::D,
                      a = sf::Keyboard::A,
                      w = sf::Keyboard::W,
                      s = sf::Keyboard::S;
    player.Movement =
        Vector2InputPtr((Vector2Input*)new FourButtonVector2Input(BoolInputPtr((BoolInput*)new KeyboardBoolInput(d)),
                                                                  BoolInputPtr((BoolInput*)new KeyboardBoolInput(a)),
                                                                  BoolInputPtr((BoolInput*)new KeyboardBoolInput(w)),
                                                                  BoolInputPtr((BoolInput*)new KeyboardBoolInput(s))));
    player.Jump = BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Key::Space,
                                                                 BoolInput::ValueStates::JustPressed));
}
void VoxelWorld::OnWorldEnd(void)
{
    for (auto element = chunkMeshes.begin(); element != chunkMeshes.end(); ++element)
    {
        delete element->second;
    }

    if (voxelMat != 0)
    {
        delete voxelMat;
    }
    if (postProcessing != 0)
    {
        delete postProcessing;
    }
    if (finalWorldRenderMat != 0)
    {
        delete finalWorldRenderMat;
    }
    if (voxelHighlightMat != 0)
    {
        delete voxelHighlightMat;
    }

    DrawingQuad::DestroyQuad();

    delete worldRenderTarget;

    voxelTex.DeleteIfValid();
    worldRenderTargetColorTex.DeleteIfValid();
    worldRenderTargetDepthTex.DeleteIfValid();
}

void VoxelWorld::OnWindowResized(unsigned int w, unsigned int h)
{
    glViewport(0, 0, w, h);
    vWindowSize.x = w;
    vWindowSize.y = h;

    player.Cam.PerspectiveInfo.Width = (float)w;
    player.Cam.PerspectiveInfo.Height = (float)h;

    worldRenderTargetColorTex.ClearData(w, h);
    worldRenderTarget->UpdateSize();
    postProcessing->OnWindowResized(w, h);
}

void VoxelWorld::UpdateWorld(float elapsed)
{
    //See if a block was hit.
    VoxelChunkManager::RayCastResult castHit = manager.CastRay(player.Cam.GetPosition(),
                                                               player.Cam.GetForward(), 100.0f);
    if (castHit.ChunkRayCastResult.CastResult.DidHitTarget)
    {
        voxelHighlightMesh.Transform.SetPosition(castHit.ChunkRayCastResult.CastResult.HitPos);
    }
    else
    {
        voxelHighlightMesh.Transform.SetPosition(Vector3f(-1, -1, -1) * 99999.0f);
    }

    //Mouse capturing.
    MouseDeltaVector2Input* ptr = (MouseDeltaVector2Input*)player.Cam.RotationInput.Input.get();
    if (capMouse)
    {
        GetWindow()->setMouseCursorVisible(false);
        ptr->MouseResetPos = GetWindow()->getPosition() +
                             sf::Vector2i(vWindowSize.x / 2, vWindowSize.y / 2);
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


    //FOV input.
    const float fovChangeSpeed = 10.0f;
    if (Input.GetBoolInputValue(INPUT_UpFov))
    {
        fov += fovChangeSpeed * elapsed;
        std::cout << fov << '\n';
    }
    if (Input.GetBoolInputValue(INPUT_DownFov))
    {
        fov -= fovChangeSpeed * elapsed;
        std::cout << fov << '\n';
    }
    player.Cam.PerspectiveInfo.SetFOVDegrees(fov);

    //Input handling.

    if (Input.GetBoolInputValue(INPUT_MouseCap))
    {
        capMouse = !capMouse;
    }
    if (capMouse)
    {
        if (Input.GetBoolInputValue(INPUT_Quit))
        {
            EndWorld();
        }
        if (Input.GetBoolInputValue(INPUT_AddVoxel))
        {
            VoxelChunkManager::RayCastResult hit = manager.CastRay(player.Cam.GetPosition(),
                                                                   player.Cam.GetForward());
            if (hit.ChunkRayCastResult.CastResult.DidHitTarget)
            {
                VoxelChunkManager::VoxelLocation toAdd =
                    manager.GetOffset(VoxelChunkManager::VoxelLocation(hit.Chunk,
                                                                       hit.ChunkRayCastResult.VoxelIndex),
                                      hit.ChunkRayCastResult.Face);
                if (toAdd.Chunk != 0)
                {
                    std::cout << "Adding a voxel. Chunk world min pos: " << toAdd.Chunk->MinCorner.x <<
                                 "," << toAdd.Chunk->MinCorner.y << "," << toAdd.Chunk->MinCorner.z <<
                                 "\nLocal voxel index: " << toAdd.LocalIndex.x << "," <<
                                 toAdd.LocalIndex.y << "," << toAdd.LocalIndex.z << "\n";

                    toAdd.Chunk->SetVoxelLocal(toAdd.LocalIndex, true);
                    chunkMeshes[manager.GetChunkIndex(toAdd.Chunk)]->RebuildMesh(true);

                    //Rebuild any chunks that the voxel was bordering on.
                    if (toAdd.LocalIndex.x == 0)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).LessX();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                        {
                            found->second->RebuildMesh(true);
                        }
                    }
                    else if (toAdd.LocalIndex.x == VoxelChunk::ChunkSize - 1)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).MoreX();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                        {
                            found->second->RebuildMesh(true);
                        }
                    }
                    if (toAdd.LocalIndex.y == 0)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).LessY();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                        {
                            found->second->RebuildMesh(true);
                        }
                    }
                    else if (toAdd.LocalIndex.y == VoxelChunk::ChunkSize - 1)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).MoreY();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                        {
                            found->second->RebuildMesh(true);
                        }
                    }
                    if (toAdd.LocalIndex.z == 0)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).LessZ();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                        {
                            found->second->RebuildMesh(true);
                        }
                    }
                    else if (toAdd.LocalIndex.z == VoxelChunk::ChunkSize - 1)
                    {
                        Vector3i loc = manager.GetChunkIndex(toAdd.Chunk).MoreZ();
                        auto found = chunkMeshes.find(loc);
                        if (found != chunkMeshes.end())
                        {
                            found->second->RebuildMesh(true);
                        }
                    }

                    if (toAdd.Chunk->MinCorner == Vector3i())
                    {
                        std::cout << "Debug output: " << DebugAssist::STR << "\n\n\n";
                    }
                }
            }
        }
        if (Input.GetBoolInputValue(INPUT_RemoveVoxel))
        {
            VoxelChunkManager::RayCastResult hit = manager.CastRay(player.Cam.GetPosition(),
                                                                   player.Cam.GetForward());
            if (hit.ChunkRayCastResult.CastResult.DidHitTarget)
            {
                std::cout << "Removing a voxel. Chunk world min pos: " << hit.Chunk->MinCorner.x <<
                              "," << hit.Chunk->MinCorner.y << "," << hit.Chunk->MinCorner.z <<
                             "\nLocal voxel index: " << hit.ChunkRayCastResult.VoxelIndex.x << "," <<
                             hit.ChunkRayCastResult.VoxelIndex.y << "," <<
                             hit.ChunkRayCastResult.VoxelIndex.z << "\n";

                hit.Chunk->SetVoxelLocal(hit.ChunkRayCastResult.VoxelIndex, false);
                chunkMeshes[manager.GetChunkIndex(hit.Chunk)]->RebuildMesh(true);

                //Rebuild any chunks that the voxel was bordering on.
                if (hit.ChunkRayCastResult.VoxelIndex.x == 0)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).LessX();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                    {
                        found->second->RebuildMesh(true);
                    }
                }
                else if (hit.ChunkRayCastResult.VoxelIndex.x == VoxelChunk::ChunkSize - 1)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).MoreX();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                    {
                        found->second->RebuildMesh(true);
                    }
                }
                if (hit.ChunkRayCastResult.VoxelIndex.y == 0)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).LessY();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                    {
                        found->second->RebuildMesh(true);
                    }
                }
                else if (hit.ChunkRayCastResult.VoxelIndex.y == VoxelChunk::ChunkSize - 1)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).MoreY();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                    {
                        found->second->RebuildMesh(true);
                    }
                }
                if (hit.ChunkRayCastResult.VoxelIndex.z == 0)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).LessZ();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                    {
                        found->second->RebuildMesh(true);
                    }
                }
                else if (hit.ChunkRayCastResult.VoxelIndex.z == VoxelChunk::ChunkSize - 1)
                {
                    Vector3i loc = manager.GetChunkIndex(hit.Chunk).MoreZ();
                    auto found = chunkMeshes.find(loc);
                    if (found != chunkMeshes.end())
                    {
                        found->second->RebuildMesh(true);
                    }
                }

                if (hit.Chunk->MinCorner == Vector3i())
                {
                    std::cout << "Debug output: " << DebugAssist::STR << "\n\n\n";
                }
            }
        }
    }
}

void VoxelWorld::RenderOpenGL(float elapsed)
{
    //Get the chunks to be drawn.
    //Draw each of the faces one at a time.
    std::vector<const Mesh*> lessX, lessY, lessZ, moreX, moreY, moreZ;
    Vector3i camPosIndex = manager.ToChunkIndex(player.Cam.GetPosition() + player.CamOffset);
    std::unordered_map<Vector3i, ChunkMesh*, Vector3i>& meshes = chunkMeshes;
    //TODO: Speed things up by keeping a persistent set of mesh vectors and only modify them when the camera moves into a new chunk.
    manager.DoToEveryChunk([camPosIndex, &meshes, &lessX, &lessY,
                            &lessZ, &moreX, &moreY, &moreZ]
                           (Vector3i chunkIndex, VoxelChunk *chnk)
                           {
                               const Mesh * msh = &meshes[chunkIndex]->GetMesh();
                               if (msh->SubMeshes[0].GetNVertices() == 0)
                               {
                                   return;
                               }

                               bool equalX = (camPosIndex.x == chunkIndex.x);
                               if (equalX || camPosIndex.x < chunkIndex.x)
                               {
                                   lessX.insert(lessX.end(), msh);
                               }
                               if (equalX || camPosIndex.x > chunkIndex.x)
                               {
                                   moreX.insert(moreX.end(), msh);
                               }

                               bool equalY = (camPosIndex.y == chunkIndex.y);
                               if (equalY || camPosIndex.y < chunkIndex.y)
                               {
                                   lessY.insert(lessY.end(), msh);
                               }
                               if (equalY || camPosIndex.y > chunkIndex.y)
                               {
                                   moreY.insert(moreY.end(), msh);
                               }

                               bool equalZ = (camPosIndex.z == chunkIndex.z);
                               if (equalZ || camPosIndex.z < chunkIndex.z)
                               {
                                   lessZ.insert(lessZ.end(), msh);
                               }
                               if (equalZ || camPosIndex.z > chunkIndex.z)
                               {
                                   moreZ.insert(moreZ.end(), msh);
                               }
                           });

    Matrix4f viewM, projM;
    player.Cam.GetViewTransform(viewM);
    player.Cam.GetPerspectiveProjection(projM);
    RenderInfo info(GetTotalElapsedSeconds(), &player.Cam, &viewM, &projM);

    //Render the world.
    worldRenderTarget->EnableDrawingInto();
    renderState.EnableState();
    ScreenClearer().ClearScreen();


    #pragma region Render each face

    const float halfVox = 0.5f * VoxelChunk::VoxelSizeF;

    voxelParams["u_surfaceNormal"].Float().SetValue(Vector3f(-1.0f, 0.0f, 0.0f));
    voxelParams["u_corner1"].Float().SetValue(Vector3f(-halfVox, -halfVox, -halfVox));
    voxelParams["u_corner2"].Float().SetValue(Vector3f(-halfVox, halfVox, -halfVox));
    voxelParams["u_corner3"].Float().SetValue(Vector3f(-halfVox, -halfVox, halfVox));
    voxelParams["u_corner4"].Float().SetValue(Vector3f(-halfVox, halfVox, halfVox));
    voxelParams["u_getQuadDecider"].Subroutine().ValueIndex = 0;
    voxelMat->Render(info, lessX, voxelParams);

    voxelParams["u_surfaceNormal"].Float().SetValue(Vector3f(0.0f, -1.0f, 0.0f));
    voxelParams["u_corner1"].Float().SetValue(Vector3f(-halfVox, -halfVox, -halfVox));
    voxelParams["u_corner2"].Float().SetValue(Vector3f(-halfVox, -halfVox, halfVox));
    voxelParams["u_corner3"].Float().SetValue(Vector3f(halfVox, -halfVox, -halfVox));
    voxelParams["u_corner4"].Float().SetValue(Vector3f(halfVox, -halfVox, halfVox));
    voxelParams["u_getQuadDecider"].Subroutine().ValueIndex = 1;
    voxelMat->Render(info, lessY, voxelParams);

    voxelParams["u_surfaceNormal"].Float().SetValue(Vector3f(0.0f, 0.0f, -1.0f));
    voxelParams["u_corner1"].Float().SetValue(Vector3f(-halfVox, -halfVox, -halfVox));
    voxelParams["u_corner2"].Float().SetValue(Vector3f(halfVox, -halfVox, -halfVox));
    voxelParams["u_corner3"].Float().SetValue(Vector3f(-halfVox, halfVox, -halfVox));
    voxelParams["u_corner4"].Float().SetValue(Vector3f(halfVox, halfVox, -halfVox));
    voxelParams["u_getQuadDecider"].Subroutine().ValueIndex = 2;
    voxelMat->Render(info, lessZ, voxelParams);

    voxelParams["u_surfaceNormal"].Float().SetValue(Vector3f(1.0f, 0.0f, 0.0f));
    voxelParams["u_corner1"].Float().SetValue(Vector3f(halfVox, -halfVox, -halfVox));
    voxelParams["u_corner2"].Float().SetValue(Vector3f(halfVox, -halfVox, halfVox));
    voxelParams["u_corner3"].Float().SetValue(Vector3f(halfVox, halfVox, -halfVox));
    voxelParams["u_corner4"].Float().SetValue(Vector3f(halfVox, halfVox, halfVox));
    voxelParams["u_getQuadDecider"].Subroutine().ValueIndex = 3;
    voxelMat->Render(info, moreX, voxelParams);

    voxelParams["u_surfaceNormal"].Float().SetValue(Vector3f(0.0f, 1.0f, 0.0f));
    voxelParams["u_corner1"].Float().SetValue(Vector3f(-halfVox, halfVox, -halfVox));
    voxelParams["u_corner2"].Float().SetValue(Vector3f(halfVox, halfVox, -halfVox));
    voxelParams["u_corner3"].Float().SetValue(Vector3f(-halfVox, halfVox, halfVox));
    voxelParams["u_corner4"].Float().SetValue(Vector3f(halfVox, halfVox, halfVox));
    voxelParams["u_getQuadDecider"].Subroutine().ValueIndex = 4;
    voxelMat->Render(info, moreY, voxelParams);

    voxelParams["u_surfaceNormal"].Float().SetValue(Vector3f(0.0f, 0.0f, 1.0f));
    voxelParams["u_corner1"].Float().SetValue(Vector3f(-halfVox, -halfVox, halfVox));
    voxelParams["u_corner2"].Float().SetValue(Vector3f(-halfVox, halfVox, halfVox));
    voxelParams["u_corner3"].Float().SetValue(Vector3f(halfVox, -halfVox, halfVox));
    voxelParams["u_corner4"].Float().SetValue(Vector3f(halfVox, halfVox, halfVox));
    voxelParams["u_getQuadDecider"].Subroutine().ValueIndex = 5;
    voxelMat->Render(info, moreZ, voxelParams);

    #pragma endregion


    voxelHighlightMat->Render(info, &voxelHighlightMesh, voxelHighlightParams);
    worldRenderTarget->DisableDrawingInto(true);

    //Render the post-process chain.
    postProcessing->RenderPostProcessing(worldRenderTarget->GetColorTextures()[0].MTex->GetTextureHandle(),
                                         worldRenderTarget->GetDepthTexture().MTex->GetTextureHandle(),
                                         player.Cam.PerspectiveInfo);
    
    //Render the final world info.
    finalWorldRenderParams["u_finalWorldRender"].Tex() =
        postProcessing->GetFinalColor().GetTextureHandle();
    ScreenClearer().ClearScreen();
    RenderingState(RenderingState::C_NONE, false, false).EnableState();
    DrawingQuad::GetInstance()->Render(info, finalWorldRenderParams, *finalWorldRenderMat);
}