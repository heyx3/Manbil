#include "AssetImporterWorld.h"

#include "../Math/NoiseGeneration.hpp"
#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/Primitives/PrimitiveGenerator.h"

#include "../DebugAssist.h"

#include <AssImp\Importer.hpp>
#include <AssImp\scene.h>
#include <AssImp\postprocess.h>

#include <iostream>



AssetImporterWorld::AssetImporterWorld(void)
    : windowSize(800, 600),
      objTex2(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, true),
      objTex3(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PixelSizes::PS_32F, true),
      cam(Vector3f(), 10.0f, 0.18f),
      SFMLOpenGLWorld(800, 600)
{

}
AssetImporterWorld::~AssetImporterWorld(void)
{

}


sf::ContextSettings AssetImporterWorld::GenerateContext(void)
{
    return sf::ContextSettings(24, 0, 0, 4, 1);
}


void AssetImporterWorld::InitializeTextures(void)
{
    objTex2.Create();
    objTex3.Create();


    //Load a 2D texture from a file.
    std::string errorMsg;
    if (!Assert(objTex2.SetDataFromFile("Content/Sample Worlds/grass.png", errorMsg),
                "Error loading 'Content/Sample Worlds/grass.png'",
                errorMsg))
    {
        return;
    }


    //Generate 3D noise into a second texture.
    //Generate a different noise value for each color component.

    Noise3D outNoise(128, 128, 128);
    Array3D<Vector4f> colors(outNoise.GetWidth(), outNoise.GetHeight(), outNoise.GetDepth(),
                             Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

    Perlin3D perl(16.0f, Perlin3D::Quintic, Vector3i(), 125123, true, Vector3u(8, 8, 8));
    
    for (int i = 0; i < 3; ++i)
    {
        perl.RandSeed = FastRand(perl.RandSeed).GetRandInt();
        perl.Generate(outNoise);
        colors.FillFunc([&outNoise, i](Vector3u loc, Vector4f* outV)
        {
            outV->operator[](i) = outNoise[loc];
        });
    }

    Assert(objTex3.SetColorData(colors),
           "Error setting RGBA data for 3D texture",
           "Unknown error; check pixel type in debugger");
}
void AssetImporterWorld::InitializeMaterials(void)
{
    SerializedMaterial matData;

    std::vector<ShaderOutput> &vertOuts = matData.MaterialOuts.VertexOutputs,
                              &fragOuts = matData.MaterialOuts.FragmentOutputs;

    //Vertex shader is a simple object-to-screen-space conversion.
    //It outputs world position, UV, and world normal to the fragment shader.
    
    matData.VertexInputs = VertexPosUVNormal::GetVertexAttributes();

    DataLine vIn_ObjPos(VertexInputNode::GetInstance(), 0),
             vIn_UV(VertexInputNode::GetInstance(), 1),
             vIn_ObjNormal(VertexInputNode::GetInstance(), 2);
    
    DataNode::Ptr objPosToWorld = SpaceConverterNode::ObjPosToWorldPos(vIn_ObjPos);
    DataLine vOut_WorldPos(objPosToWorld);
    DataNode::Ptr vOut_WorldNormal = SpaceConverterNode::ObjNormalToWorldNormal(vIn_ObjNormal);
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_WorldPos", vOut_WorldPos));
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_UV", vIn_UV));
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_WorldNormal", vOut_WorldNormal));

    DataNode::Ptr objPosToScreen = SpaceConverterNode::ObjPosToScreenPos(vIn_ObjPos);
    matData.MaterialOuts.VertexPosOutput = DataLine(objPosToScreen, 1);


    //Fragment shader multiplies the 2D texture (using UV coordinates)
    //    by the 3D texture (using world-space coordinates).
    //It also applies basic ambient/diffuse/specular lighting.

    DataLine fIn_WorldPos(FragmentInputNode::GetInstance(), 0),
             fIn_UV(FragmentInputNode::GetInstance(), 1),
             fIn_WorldNormal(FragmentInputNode::GetInstance(), 2);
    DataNode::Ptr normalizedNormal(new NormalizeNode(fIn_WorldNormal));
    DataLine lightDir(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
             ambientLight(0.65f),
             diffuseLight(0.65f),
             specLight(1.0f),
             specIntensity(64.0f);

    DataNode::Ptr tex3Coords(new MultiplyNode(0.25f, fIn_WorldPos));

    DataNode::Ptr objTex2SamplePtr(new TextureSample2DNode(fIn_UV, "u_tex2"));
    DataNode::Ptr objTex3SamplePtr(new TextureSample3DNode(tex3Coords, "u_tex3"));
    DataLine objTex2Sample(objTex2SamplePtr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels)),
             objTex3Sample(objTex3SamplePtr, TextureSample3DNode::GetOutputIndex(CO_AllColorChannels));

    DataNode::Ptr lightCalc(new LightingNode(fIn_WorldPos, normalizedNormal, lightDir,
                                             ambientLight, diffuseLight,
                                             specLight, specIntensity));
    DataLine lightBrightness(lightCalc);

    DataNode::Ptr finalColor3(new MultiplyNode(lightBrightness, objTex2Sample, objTex3Sample)),
                  finalColor4(new CombineVectorNode(finalColor3, 1.0f));

    fragOuts.push_back(ShaderOutput("fOut_FinalColor4", finalColor4));

    
    //Generate the final material.
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(matData, objParams,
                                                                                BlendMode::GetOpaque());
    if (Assert(genM.ErrorMessage.empty(), "Error generating material shaders", genM.ErrorMessage))
    {
        objMat.reset(genM.Mat);
    }


    //Set up the parameters.
    objParams["u_tex2"].Tex() = objTex2.GetTextureHandle();
    objParams["u_tex3"].Tex() = objTex3.GetTextureHandle();
}
void AssetImporterWorld::InitializeObjects(void)
{
    std::vector<VertexPosUVNormal> vertices;
    std::vector<unsigned int> indices;


    //Load a mesh using AssImp. Note that the "Importer"s destructor will clean up all meshes it loaded.
    Assimp::Importer importer;
    unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality;
    const aiScene* scene = importer.ReadFile("Content/Sample Worlds/Meshes/TestRectPrism.fbx", flags);

    //Make sure the file/mesh was loaded properly.
    if (!Assert(scene != 0,
                "Error importing mesh 'Content/Sample Worlds/Meshes/TestRectPrism.fbx'",
                importer.GetErrorString()))
    {
        return;
    }
    if (!Assert(scene->mNumMeshes == 1,
                "Invalid 'TestRectPrism.fbx' file", "should have exactly one mesh!"))
    {
        return;
    }

    aiMesh* mesh = scene->mMeshes[0];
    //Make sure the mesh is defined properly.
    if (!Assert(mesh->HasPositions(), "Error in loaded mesh", "doesn't have positions!") ||
        !Assert(mesh->HasTextureCoords(0), "Error in loaded mesh", "doesn't have UVs!") ||
        !Assert(mesh->HasNormals(), "Error in loaded mesh", "doesn't have normals!") ||
        !Assert(mesh->HasFaces(), "Error in loaded mesh", "doesn't have indices!"))
    {
        return;
    }

    //Put the vertices into the vertex array.
    vertices.resize(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        vertices[i].Pos = *(Vector3f*)(&mesh->mVertices[i].x);
        vertices[i].Normal = *(Vector3f*)(&mesh->mNormals[i].x);
        vertices[i].UV = *(Vector2f*)(&mesh->mTextureCoords[0][i].x);
    }
    //Put the indices into the indices array.
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace& fce = mesh->mFaces[i];
        if (!Assert(fce.mNumIndices == 3,
                    "Error in loaded mesh",
                    "a face has " + std::to_string(fce.mNumIndices) + " indices instead of 3!"))
        {
            return;
        }

        indices.push_back(fce.mIndices[0]);
        indices.push_back(fce.mIndices[1]);
        indices.push_back(fce.mIndices[2]);
    }


    //Create the mesh object.
    objMesh.reset(new Mesh(false, PT_TRIANGLE_LIST));
    objMesh->SetVertexData(vertices, Mesh::BUF_STATIC, VertexPosUVNormal::GetVertexAttributes());
    objMesh->SetIndexData(indices, Mesh::BUF_STATIC);

    //Set up the mesh's transform.
    objTransform.SetPosition(Vector3f(5.0f, 0.0f, 0.0f));
    objTransform.SetScale(3.0f);
}
void AssetImporterWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    if (IsGameOver())
    {
        return;
    }

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();

    cam.Window = GetWindow();

    cam.PerspectiveInfo.SetFOVDegrees(55.0f);
}


void AssetImporterWorld::OnWorldEnd(void)
{
	objMat.reset();
	objMesh.reset();

    objTex2.DeleteIfValid();
    objTex3.DeleteIfValid();
}


void AssetImporterWorld::UpdateWorld(float elapsedSeconds)
{
    cam.Update(elapsedSeconds);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
    {
        EndWorld();
        return;
    }

    //Rotate the mesh based on input.
    const float rotSpeed = 0.01f;
    Vector3f eulerRots;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        eulerRots.y += rotSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        eulerRots.y -= rotSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        eulerRots.x -= rotSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        eulerRots.x += rotSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
    {
        eulerRots.z += rotSpeed;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
    {
        eulerRots.z -= rotSpeed;
    }


    //Both of the following code blocks perform the exact same rotation.
    if (false)
    {
        objTransform.Rotate(Quaternion(Vector3f(0.0f, 0.0f, 1.0f), eulerRots.z));
        objTransform.Rotate(Quaternion(Vector3f(0.0f, 1.0f, 0.0f), eulerRots.y));
        objTransform.Rotate(Quaternion(Vector3f(1.0f, 0.0f, 0.0f), eulerRots.x));
    }
    else if (true)
    {
        objTransform.RotateAbsolute(eulerRots);
    }

    //Output the amount that was rotated.
    if (eulerRots.Length() > 0.0f)
    {
        std::cout << DebugAssist::ToString(objTransform.GetRotationAngles()) << "\n";
    }


    //Now move the mesh based on input.
    const float moveSpeed = 0.01f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
    {
		objTransform.IncrementPosition(objTransform.GetForward() * moveSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
    {
		objTransform.IncrementPosition(objTransform.GetForward() * -moveSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
    {
		objTransform.IncrementPosition(objTransform.GetRightward() * moveSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
    {
		objTransform.IncrementPosition(objTransform.GetRightward() * -moveSpeed);
    }
}


void AssetImporterWorld::RenderOpenGL(float elapsedSeconds)
{
    ScreenClearer(true, true, false, Vector4f(1.0f, 1.0f, 1.0f, 1.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE).EnableState();
    Viewport(0, 0, windowSize.x, windowSize.y).Use();

    Matrix4f viewM, projM;
    cam.GetViewTransform(viewM);
    cam.GetPerspectiveProjection(projM);

    RenderInfo camInfo(GetTotalElapsedSeconds(), &cam, &viewM, &projM);
    objMat->Render(*objMesh, objTransform, camInfo, objParams);
}


void AssetImporterWorld::OnInitializeError(std::string errorMsg)
{
	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	EndWorld();

	std::cout << "Enter any key to continue:\n";
    char dummy;
    std::cin >> dummy;
}
void AssetImporterWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    cam.PerspectiveInfo.Width = newW;
    cam.PerspectiveInfo.Height = newH;

    windowSize.x = newW;
    windowSize.y = newH;
}


bool AssetImporterWorld::Assert(bool test, std::string errorIntro, const std::string& error)
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