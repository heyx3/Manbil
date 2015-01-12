#include "AssImpTestWorld.h"

#include "../Math/NoiseGeneration.hpp"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/PrimitiveGenerator.h"

#include "../DebugAssist.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>



AssImpTestWorld::AssImpTestWorld(void)
    : objMat(0), objMesh(TriangleList), windowSize(800, 600),
      objTex2(TextureSampleSettings2D(FT_LINEAR, WT_WRAP), PixelSizes::PS_8U, true),
      objTex3(TextureSampleSettings3D(FT_LINEAR, WT_WRAP), PixelSizes::PS_32F, true),
      cam(Vector3f(), 10.0f, 0.06f),
      SFMLOpenGLWorld(800, 600, sf::ContextSettings(24, 0, 0, 4, 1))
{

}
AssImpTestWorld::~AssImpTestWorld(void)
{

}

void AssImpTestWorld::InitializeTextures(void)
{
    objTex2.Create();
    objTex3.Create();


    //Just load the 2D texture from a file.
    std::string errorMsg;
    if (!Assert(objTex2.SetDataFromFile("Content/Textures/CheckboxBackground.png", errorMsg),
                "Error loading 'Content/Textures/CheckboxBackground.png'",
                errorMsg))
    {
        return;
    }


    //Generate 3D noise for the texture. Generate three different noise values for each color component.

    Noise3D outNoise(128, 128, 128);
    Array3D<Vector4f> colors(outNoise.GetWidth(), outNoise.GetHeight(), outNoise.GetDepth(),
                             Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

    Perlin3D perl(16.0f, Perlin3D::Quintic, Vector3i(), 125123, true, Vector3u(8, 8, 8));
    
    //For each component (xyz/rgb), generate noise into each pixel.
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
void AssImpTestWorld::InitializeMaterials(void)
{
    DataNode::ClearMaterialData();

    std::vector<ShaderOutput> &vertOuts = DataNode::MaterialOuts.VertexOutputs,
                              &fragOuts = DataNode::MaterialOuts.FragmentOutputs;

    //Vertex shader is a simple object-to-screen-space conversion.
    //It outputs world position, UV, and world normal to the fragment shader.
    
    DataNode::VertexIns = VertexPosTex1Normal::GetAttributeData();

    DataLine vIn_ObjPos(VertexInputNode::GetInstance(), 0),
             vIn_UV(VertexInputNode::GetInstance(), 1),
             vIn_ObjNormal(VertexInputNode::GetInstance(), 2);
    
    DataNode::Ptr objPosToWorld = SpaceConverterNode::ObjPosToWorldPos(vIn_ObjPos, "objPosToWorld");
    DataLine vOut_WorldPos(objPosToWorld);
    DataNode::Ptr vOut_WorldNormal(new SpaceConverterNode(vIn_ObjNormal,
                                                          SpaceConverterNode::ST_OBJECT,
                                                          SpaceConverterNode::ST_WORLD,
                                                          SpaceConverterNode::DT_NORMAL,
                                                          "objNormalToWorld"));
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_WorldPos", vOut_WorldPos));
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_UV", vIn_UV));
    vertOuts.insert(vertOuts.end(), ShaderOutput("vOut_WorldNormal", vOut_WorldNormal));

    DataNode::Ptr objPosToScreen = SpaceConverterNode::ObjPosToScreenPos(vIn_ObjPos, "objPosToScreen");
    DataNode::MaterialOuts.VertexPosOutput = DataLine(objPosToScreen, 1);


    //Fragment shader multiplies the 2D texture (using UV coordinates)
    //    by the 3D texture (using world-space coordinates).
    //It also applies basic ambient/diffuse/specular lighting.

    DataLine fIn_WorldPos(FragmentInputNode::GetInstance(), 0),
             fIn_UV(FragmentInputNode::GetInstance(), 1),
             fIn_WorldNormal(FragmentInputNode::GetInstance(), 2);
    DataNode::Ptr normalizedNormal(new NormalizeNode(fIn_WorldNormal, "NormalizedNormal"));
    DataLine lightDir(Vector3f(-1.0f, -1.0f, -1.0f).Normalized()),
             ambientLight(0.35f),
             diffuseLight(0.65f),
             specLight(1.0f),
             specIntensity(64.0f);

    DataNode::Ptr tex3Coords(new MultiplyNode(0.25f, fIn_WorldPos, "tex3Coords"));

    DataNode::Ptr objTex2SamplePtr(new TextureSample2DNode(fIn_UV, "u_tex2", "tex2Sampler"));
    DataNode::Ptr objTex3SamplePtr(new TextureSample3DNode(tex3Coords, "u_tex3", "tex3Sampler"));
    DataLine objTex2Sample(objTex2SamplePtr, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels)),
             objTex3Sample(objTex3SamplePtr, TextureSample3DNode::GetOutputIndex(CO_AllColorChannels));

    DataNode::Ptr lightCalc(new LightingNode(fIn_WorldPos, normalizedNormal, lightDir,
                                             "lightCalc", ambientLight, diffuseLight,
                                             specLight, specIntensity));
    DataLine lightBrightness(lightCalc);

    DataNode::Ptr finalColor3(new MultiplyNode(lightBrightness, objTex2Sample,
                                              objTex3Sample, "finalColor3")),
                  finalColor4(new CombineVectorNode(finalColor3, 1.0f, "finalColor4"));

    fragOuts.insert(fragOuts.end(), ShaderOutput("fOut_FinalColor4", finalColor4));

    
    //Generate the final material.
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(objParams, RM_Opaque);
    if (Assert(genM.ErrorMessage.empty(), "Error generating material shaders", genM.ErrorMessage))
    {
        objMat = genM.Mat;
    }


    //Set up the parameters.
    objParams.Texture2DUniforms["u_tex2"].Texture = objTex2.GetTextureHandle();
    objParams.Texture3DUniforms["u_tex3"].Texture = objTex3.GetTextureHandle();
}
void AssImpTestWorld::InitializeObjects(void)
{
    //Generate the mesh data.
    std::vector<VertexPosTex1Normal> vertices;
    std::vector<unsigned int> indices;
    if (false)
    {
        //Generate a cube.
        PrimitiveGenerator::GenerateCube(vertices, indices, false, false);
    }
    else if (true)
    {
        //Load a mesh using AssImp. The "Importer"s destructor will clean up all meshes it loaded.
        Assimp::Importer importer;
        unsigned int flags = aiProcessPreset_TargetRealtime_MaxQuality;
        const aiScene* scene = importer.ReadFile("Content/Meshes/TestRectPrism.fbx", flags);

        //Make sure the mesh was loaded properly.
        if (!Assert(scene != 0,
                    "Error importing mesh 'Content/Meshes/TestRectPrism.fbx'",
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
        for (int i = 0; i < mesh->mNumVertices; ++i)
        {
            vertices[i].Pos = *(Vector3f*)(&mesh->mVertices[i].x);
            vertices[i].Normal = *(Vector3f*)(&mesh->mNormals[i].x);
            vertices[i].TexCoords = *(Vector2f*)(&mesh->mTextureCoords[0][i].x);
        }
        //Put the indices into the indices array.
        for (int i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace& fce = mesh->mFaces[i];
            if (!Assert(fce.mNumIndices == 3,
                        "Error in loaded mesh",
                        "a face has " + std::to_string(fce.mNumIndices) + " indices instead of 3!"))
            {
                return;
            }

            indices.insert(indices.end(), fce.mIndices[0]);
            indices.insert(indices.end(), fce.mIndices[1]);
            indices.insert(indices.end(), fce.mIndices[2]);
        }
    }


    //Create the mesh object.

    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer(vbo, vertices.data(), vertices.size());
    RenderDataHandler::CreateIndexBuffer(ibo, indices.data(), indices.size());

    VertexIndexData vid(vertices.size(), vbo, indices.size(), ibo);
    objMesh.SubMeshes.insert(objMesh.SubMeshes.end(), vid);


    //Set up the mesh's transform.
    objMesh.Transform.SetPosition(Vector3f(5.0f, 0.0f, 0.0f));
}
void AssImpTestWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    if (IsGameOver()) return;

    InitializeStaticSystems(false, false, false);

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();

    cam.Window = GetWindow();

    cam.PerspectiveInfo.SetFOVDegrees(55.0f);
}

void AssImpTestWorld::OnWorldEnd(void)
{
    delete objMat;
    objTex2.DeleteIfValid();
    objTex3.DeleteIfValid();

    DestroyStaticSystems(false, false, false);
}

void AssImpTestWorld::UpdateWorld(float elapsedSeconds)
{
    if (cam.Update(elapsedSeconds))
    {
        EndWorld();
        return;
    }


    const float rotSpeed = 0.01f;
    Vector3f eulerRots;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        eulerRots.y += rotSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        eulerRots.y -= rotSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        eulerRots.x -= rotSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        eulerRots.x += rotSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
        eulerRots.z += rotSpeed;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
        eulerRots.z -= rotSpeed;

    //All of the following rotation sequences are identical.
    if (false)
    {
        objMesh.Transform.Rotate(Quaternion(Vector3f(0.0f, 0.0f, 1.0f), eulerRots.z));
        objMesh.Transform.Rotate(Quaternion(Vector3f(0.0f, 1.0f, 0.0f), eulerRots.y));
        objMesh.Transform.Rotate(Quaternion(Vector3f(1.0f, 0.0f, 0.0f), eulerRots.x));
    }
    else if (true)
    {
        objMesh.Transform.RotateAbsolute(eulerRots);
    }


    if (eulerRots.Length() > 0.0f)
    {
        std::cout << DebugAssist::ToString(objMesh.Transform.GetRotationAngles()) << "\n";
    }


    const float moveSpeed = 0.01f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        objMesh.Transform.IncrementPosition(objMesh.Transform.GetForward() * moveSpeed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
        objMesh.Transform.IncrementPosition(objMesh.Transform.GetForward() * -moveSpeed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
        objMesh.Transform.IncrementPosition(objMesh.Transform.GetRightward() * moveSpeed);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
        objMesh.Transform.IncrementPosition(objMesh.Transform.GetRightward() * -moveSpeed);
}

bool AssImpTestWorld::RenderWorldGeometry(const RenderInfo& info)
{
    ScreenClearer().ClearScreen();
    RenderingState(true, true, RenderingState::C_NONE).EnableState();

    std::vector<const Mesh*> toDraw;
    toDraw.insert(toDraw.end(), &objMesh);

    return objMat->Render(info, toDraw, objParams);
}
void AssImpTestWorld::RenderOpenGL(float elapsedSeconds)
{
    glViewport(0, 0, windowSize.x, windowSize.y);

    Matrix4f worldM, viewM, projM;
    TransformObject dummy;

    cam.GetViewTransform(viewM);
    cam.GetPerspectiveTransform(projM);

    RenderInfo info(this, &cam, &dummy, &worldM, &viewM, &projM);
    if (!Assert(RenderWorldGeometry(info), "Error rendering world", objMat->GetErrorMsg()))
    {
        return;
    }
}

void AssImpTestWorld::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	std::cout << "Enter any key to continue:\n";
    char dummy;
    std::cin >> dummy;
}
void AssImpTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    cam.PerspectiveInfo.Width = newW;
    cam.PerspectiveInfo.Height = newH;

    glViewport(0, 0, newW, newH);

    windowSize.x = newW;
    windowSize.y = newH;
}

bool AssImpTestWorld::Assert(bool test, std::string errorIntro, const std::string& error)
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