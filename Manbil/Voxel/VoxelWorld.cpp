#include "VoxelWorld.h"

#include "../Math/Higher Math/GeometricMath.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../ScreenClearer.h"
#include "../Math/Shapes/ThreeDShapes.h"
#include "../Input/Input Objects/MouseBoolInput.h"


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
const unsigned int INPUT_AddVoxel = 123753;


VoxelWorld::VoxelWorld(void)
    : SFMLOpenGLWorld(vWindowSize.x, vWindowSize.y, sf::ContextSettings(8, 0, 0, 3, 1)),
        chunk(Vector3i(0, 0, 0)), voxelMat(0), renderState(),
        cam(5.0f, 0.06), voxelMesh(PrimitiveTypes::Triangles),
        light(0.8f, 0.2f, Vector3f(1, 1, 1), Vector3f(-1, -1, -1).Normalized())
{
}

void VoxelWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    //Input.

    Input.AddBoolInput(INPUT_AddVoxel, BoolInputPtr(new MouseBoolInput(sf::Mouse::Left, BoolInput::ValueStates::JustPressed)));


    //Initialize the chunk mesh.

    chunk.SetVoxelLocal(Vector3i(0, 0, 0), true);
    Sphere spher(Vector3f(15.0f, 15.0f, 15.0f), 5.0f);
    chunk.SetVoxels(spher, true);
    Capsule caps(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 15.0f), 3.0f);
    chunk.SetVoxels(caps, true);

    std::vector<Vector3f> vertices;
    std::vector<unsigned int> indices;
    chunk.BuildTriangles(vertices, indices, 0, 0, 0, 0, 0, 0);

    Vector3f * normals = new Vector3f[vertices.size()];
    GeometricMath::CalculateNormals(vertices.data(), indices.data(), vertices.size(), indices.size(), normals);
    Vertex * verts = new Vertex[vertices.size()];
    for (int i = 0; i < vertices.size(); ++i)
    {
        verts[i] = Vertex(vertices[i], Vector2f(), normals[i]);
    }
    delete[] normals;    

    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer(vbo, verts, vertices.size(), RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(ibo, indices.data(), indices.size(), RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    delete[] verts;

    voxelMesh.SetVertexIndexData(VertexIndexData(vertices.size(), vbo, indices.size(), ibo));
    voxelMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_AmbientName].SetValue(light.AmbientIntensity);
    voxelMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DiffuseName].SetValue(light.DiffuseIntensity);
    voxelMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DirName].SetValue(light.Direction);
    voxelMesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_ColorName].SetValue(light.Color);


    //Initialize the material.
    std::unordered_map<RenderingChannels, DataLine> channels;
    channels[RenderingChannels::RC_Diffuse] = DataLine(Vector3f(1.0f, 1.0f, 1.0f));
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
    std::unordered_map<std::string, UniformValueF> & fUnis = voxelMesh.Uniforms.FloatUniforms;
    const std::vector<UniformList::Uniform> & unis = voxelMat->GetUniforms(RenderPasses::BaseComponents).FloatUniforms;
    fUnis[MaterialConstants::DirectionalLight_AmbientName].Location = UniformList::FindUniform(MaterialConstants::DirectionalLight_AmbientName, unis).Loc;
    fUnis[MaterialConstants::DirectionalLight_DiffuseName].Location = UniformList::FindUniform(MaterialConstants::DirectionalLight_DiffuseName, unis).Loc;
    fUnis[MaterialConstants::DirectionalLight_ColorName].Location = UniformList::FindUniform(MaterialConstants::DirectionalLight_ColorName, unis).Loc;
    fUnis[MaterialConstants::DirectionalLight_DirName].Location = UniformList::FindUniform(MaterialConstants::DirectionalLight_DirName, unis).Loc;
    

    cam.SetPosition(Vector3f(2, 2, 2));
    cam.SetRotation(Vector3f(-10, -10, -10).Normalized(), Vector3f(0.0f, 0.0f, 1.0f), true);
    cam.Window = GetWindow();
    cam.Info.SetFOVDegrees(55.0f);
    cam.Info.Width = vWindowSize.x;
    cam.Info.Height = vWindowSize.y;
    cam.Info.zNear = 0.1f;
    cam.Info.zFar = 1000.0f;
}
void VoxelWorld::OnWorldEnd(void)
{
    if (voxelMat != 0) delete voxelMat;
}

void VoxelWorld::OnWindowResized(unsigned int w, unsigned int h)
{
    glViewport(0, 0, w, h);
    vWindowSize.x = w;
    vWindowSize.y = h;
}

void VoxelWorld::UpdateWorld(float elapsed)
{
    if (cam.Update(elapsed))
        EndWorld();


}

void VoxelWorld::RenderOpenGL(float elapsed)
{
    renderState.EnableState();
    ScreenClearer(true, true, false, Vector4f(0.0f, 0.0f, 0.0f, 0.0f)).ClearScreen();

    std::vector<const Mesh*> meshes;
    meshes.insert(meshes.end(), &voxelMesh);

    TransformObject dummy;

    Matrix4f worldM, viewM, projM;
    worldM.SetAsIdentity();
    cam.GetViewTransform(viewM);
    projM.SetAsPerspProj(cam.Info);
    RenderInfo info(this, &cam, &dummy, &worldM, &viewM, &projM);

    if (!voxelMat->Render(RenderPasses::BaseComponents, info, meshes))
    {
        PrintError("Error rendering voxel material", voxelMat->GetErrorMsg());
        EndWorld();
    }
}