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
    if (!quadTex.loadFromFile("Content/Textures/Grass.png"))
    {
        std::cout << "Failed to load 'Grass.png'.\n";
        Pause();
        EndWorld();
        return;
    }
    quadTex.setRepeated(true);
    quadTex.setSmooth(true);
    sf::Texture::bind(&quadTex);
    TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP, true).SetData();
}
void OpenGLTestWorld::InitializeMaterials(void)
{
    typedef DataNodePtr DNP;
    typedef RenderingChannels RC;

    std::unordered_map<RC, DataLine> chs;

    //Diffuse channel just uses a scaled texture.
    std::vector<DataLine> mvInputs;
    TextureSampleNode * tsn = new TextureSampleNode(DataLine(DNP(new UVNode()), 0), DataLine(Vector(10.0f, 5.0f)));
    texSamplerName = tsn->GetSamplerUniformName();
    DNP tsnDNP(tsn);
    chs[RC::RC_Diffuse] = DataLine(tsnDNP, TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    
    //Specular channel is a constant value.
    chs[RC::RC_Specular] = DataLine(DNP(new OneMinusNode(DataLine(tsnDNP, TextureSampleNode::GetOutputIndex(ChannelsOut::CO_Green)))), 0);
    chs[RC::RC_SpecularIntensity] = DataLine(Vector(64.0f));



    std::string vs, fs;
    UniformDictionary uniforms;
    ShaderGenerator::GenerateShaders(vs, fs, uniforms, RenderingModes::RM_Opaque, true, LightSettings(false), chs);

    quadMat = new Material(vs, fs, uniforms, RenderingModes::RM_Opaque, false, LightSettings(false));
    if (quadMat->HasError())
    {
        std::cout << "Error creating quad material: " << quadMat->GetErrorMsg() << "\n";
        Pause();
        EndWorld();
        return;
    }
}
void OpenGLTestWorld::InitializeObjects(void)
{
    quad = new DrawingQuad();
    quad->SetSize(Vector2f(50.0f, 50.0f));

    const UniformList & uniforms = quadMat->GetUniforms(RenderPasses::BaseComponents);

    UniformList::Uniform texSampler = UniformList::FindUniform(texSamplerName, uniforms.TextureUniforms);
    quad->GetMesh().Uniforms.TextureUniforms[texSamplerName] = UniformSamplerValue(&quadTex, texSampler.Loc, texSampler.Name);

    UniformList::Uniform unfVal = UniformList::FindUniform(MC::DirectionalLight_AmbientName, uniforms.FloatUniforms);
    quad->GetMesh().Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_AmbientName] = UniformValue(dirLight.AmbientIntensity, unfVal.Loc, unfVal.Name);
    unfVal = UniformList::FindUniform(MC::DirectionalLight_DiffuseName, uniforms.FloatUniforms);
    quad->GetMesh().Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DiffuseName] = UniformValue(dirLight.DiffuseIntensity, unfVal.Loc, unfVal.Name);
    unfVal = UniformList::FindUniform(MC::DirectionalLight_ColorName, uniforms.FloatUniforms);
    quad->GetMesh().Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_ColorName] = UniformValue(dirLight.Color, unfVal.Loc, unfVal.Name);
    unfVal = UniformList::FindUniform(MC::DirectionalLight_DirName, uniforms.FloatUniforms);
    quad->GetMesh().Uniforms.FloatUniforms[MaterialConstants::DirectionalLight_DirName] = UniformValue(dirLight.Direction, unfVal.Loc, unfVal.Name);
}


OpenGLTestWorld::OpenGLTestWorld(void)
: SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(24, 0, 4, 3, 3))
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
    DeleteAndSetToNull(quad);
    DeleteAndSetToNull(quadMat);
}
void OpenGLTestWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(quad);
    DeleteAndSetToNull(quadMat);
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
    if (!quad->Render(RenderPasses::BaseComponents, info, *quadMat))
    {
        std::cout << "Error rendering quad: " << quadMat->GetErrorMsg() << ".\n";
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