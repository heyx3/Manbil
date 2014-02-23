#include "OpenGLTestWorld.h"

#include <iostream>

#include "Material.h"
#include "ScreenClearer.h"
#include "RenderingState.h"
#include "TextureSettings.h"
#include "Input/Input Objects/MouseBoolInput.h"
#include "Math/Higher Math/BumpmapToNormalmap.h"

#include "Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "Math/NoiseGeneration.hpp"

#include <assert.h>

typedef MaterialConstants MC;

namespace OGLTestPrints
{
	bool PrintRenderError(const char * errorIntro)
{
	const char * error = GetCurrentRenderingError();
	if (strcmp(error, "") != 0)
	{
		std::cout << errorIntro << ": " << error << "\n";
		ClearAllRenderingErrors();
		return false;
	}

	return true;
}

	void Pause(void)
	{
		char dummy;
		std::cin >> dummy;
	}
}
using namespace OGLTestPrints;



Vector2i windowSize(1000, 1000);
const RenderingState worldRenderState;
std::string texSamplerName = "";


void OpenGLTestWorld::InitializeTextures(void)
{
    if (!diffuseTex.loadFromFile("Content/Textures/Grass.png"))
    {
        std::cout << "Failed to load 'Grass.png'.\n";
        Pause();
        EndWorld();
        return;
    }
    diffuseTex.setRepeated(true);
    diffuseTex.setSmooth(true);
    sf::Texture::bind(&diffuseTex);
    TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP, true).SetData();
}
void OpenGLTestWorld::InitializeMaterials(void)
{
    typedef DataNodePtr DNP;
    typedef RenderingChannels RC;

    std::unordered_map<RC, DataLine> chs;

    //Diffuse channel just uses a scaled texture.
    std::vector<DataLine> mvInputs;
    TextureSampleNode * tsn = new TextureSampleNode(DataLine(DNP(new UVNode()), 0), DataLine(VectorF(70.0f, 70.0f)));
    texSamplerName = tsn->GetSamplerUniformName();
    DNP tsnDNP(tsn);
    chs[RC::RC_Diffuse] = DataLine(tsnDNP, TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    
    //Specular channel is a constant value.
    chs[RC::RC_Specular] = DataLine(VectorF(1.0f));
    chs[RC::RC_SpecularIntensity] = DataLine(VectorF(32.0f));



    std::string vs, fs;
    UniformDictionary uniforms;
    ShaderGenerator::GenerateShaders(vs, fs, uniforms, RenderingModes::RM_Opaque, true, LightSettings(false), chs);

    mat = new Material(vs, fs, uniforms, RenderingModes::RM_Opaque, false, LightSettings(false));
    if (mat->HasError())
    {
        std::cout << "Error creating quad material: " << mat->GetErrorMsg() << "\n";
        Pause();
        EndWorld();
        return;
    }
}
void OpenGLTestWorld::InitializeObjects(void)
{
    const unsigned int size = 300;
    Noise2D heightmap(size, size);
    float worldSize = 1.0f,
          worldHeight = 15.0f;


    #pragma region Terrain Heightmap

    Perlin per(35.0f, Perlin::Smoothness::Quintic);
    per.Generate(heightmap);

    #pragma endregion


    terr = new Terrain(300);
    terr->SetHeightmap(heightmap);

    unsigned int nVs = terr->GetVerticesCount();
    terrPoses = new Vector3f[nVs];
    Vector2f * terrUVs = new Vector2f[nVs];
    Vector3f * terrNormals = new Vector3f[nVs];
    unsigned int * indices = new unsigned int[terr->GetIndicesCount()];

    terr->CreateVertexPositions(terrPoses);
    terr->CreateVertexNormals(terrNormals, terrPoses, Vector3f(worldSize, worldSize, worldHeight));
    terr->CreateVertexTexCoords(terrUVs);
    terr->CreateVertexIndices(indices);

    Vertex * vertices = new Vertex[nVs];
    for (unsigned int i = 0; i < nVs; ++i)
    {
        vertices[i] = Vertex(terrPoses[i].ComponentProduct(Vector3f(worldSize, worldSize, worldHeight)), terrUVs[i], terrNormals[i]);
    }

    RenderObjHandle vbo, ibo;
    RenderDataHandler::CreateVertexBuffer(vbo, vertices, nVs, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    RenderDataHandler::CreateIndexBuffer(ibo, indices, terr->GetIndicesCount(), RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
    VertexIndexData vid(nVs, vbo, terr->GetIndicesCount(), ibo);
    terrMesh.SetVertexIndexData(vid);
    
    delete[] terrUVs, terrNormals, indices, vertices;


    const UniformList & uniforms = mat->GetUniforms(RenderPasses::BaseComponents);
    Mesh & mesh = terrMesh;

    UniformList::Uniform texSampler = UniformList::FindUniform(texSamplerName, uniforms.TextureUniforms);
    terrMesh.Uniforms.TextureUniforms[texSamplerName] = UniformSamplerValue(&diffuseTex, texSampler.Loc, texSampler.Name);

    UniformList::Uniform unfVal = UniformList::FindUniform(MC::DirectionalLight_AmbientName, uniforms.FloatUniforms);
    mesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_AmbientName] = UniformValueF(dirLight.AmbientIntensity, unfVal.Loc, unfVal.Name);
    unfVal = UniformList::FindUniform(MC::DirectionalLight_DiffuseName, uniforms.FloatUniforms);
    mesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DiffuseName] = UniformValueF(dirLight.DiffuseIntensity, unfVal.Loc, unfVal.Name);
    unfVal = UniformList::FindUniform(MC::DirectionalLight_ColorName, uniforms.FloatUniforms);
    mesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_ColorName] = UniformValueF(dirLight.Color, unfVal.Loc, unfVal.Name);
    unfVal = UniformList::FindUniform(MC::DirectionalLight_DirName, uniforms.FloatUniforms);
    mesh.Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DirName] = UniformValueF(dirLight.Direction, unfVal.Loc, unfVal.Name);
}


OpenGLTestWorld::OpenGLTestWorld(void)
: SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(24, 0, 4, 3, 3)), terr(0), terrPoses(0), mat(0), terrMesh(PrimitiveTypes::Triangles)
{
	dirLight.Direction = Vector3f(1.0f, 1.0f, -1.0f).Normalized();
	dirLight.Color = Vector3f(1.0f, 1.0f, 1.0f);

	dirLight.AmbientIntensity = 0.1f;
	dirLight.DiffuseIntensity = 0.8f;
}
void OpenGLTestWorld::InitializeWorld(void)
{
	SFMLOpenGLWorld::InitializeWorld();
	if (IsGameOver()) return;
	

	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();


    //Camera.
    Vector3f pos(2.0f, 2.0f, 10.0f);
    cam.SetPosition(pos);
    cam.SetRotation(-pos, Vector3f(0.0f, 0.0f, 1.0f), false);
    cam.Window = GetWindow();
    cam.Info.FOV = ToRadian(55.0f);
    cam.Info.zFar = 10000.0f;
    cam.Info.zNear = 1.0f;
    cam.Info.Width = windowSize.x;
    cam.Info.Height = windowSize.y;
}

OpenGLTestWorld::~OpenGLTestWorld(void)
{
    DeleteAndSetToNull(terr);
    DeleteAndSetToNull(terrPoses);
    DeleteAndSetToNull(mat);
}
void OpenGLTestWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(terr);
    DeleteAndSetToNull(terrPoses);
    DeleteAndSetToNull(mat);
}

void OpenGLTestWorld::OnInitializeError(std::string errorMsg)
{
	EndWorld();

	SFMLOpenGLWorld::OnInitializeError(errorMsg);

	std::cout << "Enter any key to continue:\n";
	Pause();
}


void OpenGLTestWorld::UpdateWorld(float elapsedSeconds)
{
    //quad->SetSize(Vector2f(3.0f, 10.0f * sinf(GetTotalElapsedSeconds())));
    //quad->GetMesh().Uniforms.FloatUniforms[diffuseSpeedName].Value[0] = BasicMath::Max(0.001f, cam.GetPosition().x);
    //quad->GetMesh().Transform.Rotate(Vector3f(elapsedSeconds * 0.30f, 0.0f, 0.0f));

	if (cam.Update(elapsedSeconds))
	{
		EndWorld();
	}
}

void OpenGLTestWorld::RenderWorldGeometry(const RenderInfo & info)
{
    std::vector<const Mesh *> meshes;
    meshes.insert(meshes.end(), &terrMesh);

    if (!mat->Render(RenderPasses::BaseComponents, info, meshes))
    {
        std::cout << "Error rendering world geometry: " << mat->GetErrorMsg() << ".\n";
        Pause();
        EndWorld();
        return;
    }
}

void OpenGLTestWorld::RenderOpenGL(float elapsedSeconds)
{
	Matrix4f worldM, viewM, projM;
	TransformObject dummy;

	worldM.SetAsIdentity();
	cam.GetViewTransform(viewM);
	projM.SetAsPerspProj(cam.Info);
    //cam.GetOrthoProjection(projM);

	RenderInfo info((SFMLOpenGLWorld*)this, (Camera*)&cam, &dummy, &worldM, &viewM, &projM);
	
    //Draw the world.
	ScreenClearer().ClearScreen();
    worldRenderState.EnableState();
	RenderWorldGeometry(info);
}


void OpenGLTestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
	ClearAllRenderingErrors();

	glViewport(0, 0, newW, newH);
	cam.Info.Width = newW;
	cam.Info.Height = newH;
    windowSize.x = newW;
    windowSize.y = newH;
}