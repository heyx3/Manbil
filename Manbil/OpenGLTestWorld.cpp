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



Vector2i windowSize(100, 100);
const RenderingState worldRenderState;
std::string texSamplerName = "";


void OpenGLTestWorld::InitializeTextures(void)
{
    if (!myTex.loadFromFile("Content/Textures/Normalmap.png"))
    {
        std::cout << "Failed to load 'Normalmap.png'.\n";
        Pause();
        EndWorld();
        return;
    }
    sf::Texture::bind(&myTex);
    TextureSettings(TextureSettings::TextureFiltering::TF_LINEAR, TextureSettings::TextureWrapping::TW_WRAP, true).SetData();
}
void OpenGLTestWorld::InitializeMaterials(void)
{
    typedef DataNodePtr DNP;
    typedef RenderingChannels RC;

    TextureSampleNode * normalMap = new TextureSampleNode(DataLine(DataNodePtr(new UVNode()), 0),
                                                          DataLine(VectorF(20.0f, 20.0f)),
                                                          DataLine(VectorF(0.01f, 0.01f)));
    texSamplerName = normalMap->GetSamplerUniformName();

    channels[RC::RC_Diffuse] = DataLine(VectorF(Vector3f(0.275f, 0.275f, 1.0f)));
    channels[RC::RC_Normal] = DataLine(DataNodePtr(normalMap), TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    channels[RC::RC_Specular] = DataLine(VectorF(2.0f));
    channels[RC::RC_SpecularIntensity] = DataLine(VectorF(256.0f));
}
void OpenGLTestWorld::InitializeObjects(void)
{
    const unsigned int size = 300;

    water = new Water(size, Vector3f(0.0f, 0.0f, 0.0f),
                      OptionalValue<Water::RippleWaterCreationArgs>(Water::RippleWaterCreationArgs(1)),
                      OptionalValue<Water::DirectionalWaterCreationArgs>(Water::DirectionalWaterCreationArgs(2)),
                      OptionalValue<Water::SeedmapWaterCreationArgs>(),
                      RenderingModes::RM_Opaque, true, LightSettings(false), channels);
    if (water->HasError())
    {
        std::cout << "Error creating water: " << water->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }

    water->SetLighting(dirLight);
    const Material * waterMat = water->GetMaterial();
    water->GetMesh().Uniforms.TextureUniforms[texSamplerName] =
        UniformSamplerValue(&myTex, texSamplerName,
                            waterMat->GetUniforms(RenderPasses::BaseComponents).FindUniform(texSamplerName, waterMat->GetUniforms(RenderPasses::BaseComponents).TextureUniforms).Loc);

    water->AddRipple(Water::RippleWaterArgs(Vector3f(), 150.0f, 4.0f, 5.0f, 10.0f));
    water->AddFlow(Water::DirectionalWaterArgs(Vector2f(4.0f, 0.0f), 5.0f, 10.0f));
    water->AddFlow(Water::DirectionalWaterArgs(Vector2f(0.0f, -16.0f), 0.1f, 1.5f));
}


OpenGLTestWorld::OpenGLTestWorld(void)
: SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(24, 0, 4, 3, 3)), water(0)
{
	dirLight.Direction = Vector3f(0.6f, 0.6f, -1.0f).Normalized();
	dirLight.Color = Vector3f(1.0f, 1.0f, 1.0f);

	dirLight.AmbientIntensity = 0.0f;
	dirLight.DiffuseIntensity = 1.0f;
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
    cam.SetMoveSpeed(30.0f);
    cam.SetRotSpeed(0.25f);
}

OpenGLTestWorld::~OpenGLTestWorld(void)
{
    DeleteAndSetToNull(water);
}
void OpenGLTestWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(water);
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
	if (cam.Update(elapsedSeconds))
	{
		EndWorld();
	}
    water->Update(elapsedSeconds);
}

void OpenGLTestWorld::RenderWorldGeometry(const RenderInfo & info)
{
    if (!water->Render(info))
    {
        std::cout << "Error rendering world geometry: " << water->GetErrorMessage() << ".\n";
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