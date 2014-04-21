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


Vector2i windowSize(250, 250);
const RenderingState worldRenderState;
std::string texSamplerName = "";
const unsigned int maxRipples = 3,
                   maxFlows = 2;


void OpenGLTestWorld::InitializeTextures(void)
{
    worldRenderID = manager.CreateRenderTarget(windowSize.x, windowSize.y, true, true);
    if (worldRenderID == RenderTargetManager::ERROR_ID)
    {
        std::cout << "Error creating world render target: " << manager.GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }

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


    #pragma region Water


    //Vertex output 1: object-space position.
    //Vertex output 2: UV coords.
    //Vertex output 3: color.
    //Vertex output 4: world-space position.

    DNP waterNode(new WaterNode(RenderingChannels::RC_VERTEX_OUT_1, 3, 2));
    channels[RC::RC_VERTEX_OUT_1] = DataLine(waterNode, WaterNode::GetVertexPosOutputIndex());
    channels[RC::RC_VERTEX_OUT_2] = DataLine(DNP(new UVNode()), 0);
    channels[RC::RC_VERTEX_OUT_3] = DataLine(DNP(new ObjectColorNode()), 0);
    channels[RC::RC_VERTEX_OUT_4] = DataLine(DNP(new ObjectPosToWorldPosCalcNode(channels[RC::RC_VERTEX_OUT_1])), 0);

    DNP waterSurfaceDistortion(new WaterSurfaceDistortNode(WaterSurfaceDistortNode::GetWaterSeedIn(RC::RC_VERTEX_OUT_3),
                                                           DataLine(0.01f), DataLine(0.5f),
                                                           WaterSurfaceDistortNode::GetTimeIn(RC::RC_VERTEX_OUT_3)));
    DNP normalMap(TextureSampleNode::CreateComplexTexture(DataLine(waterSurfaceDistortion, 0),
                                                          "u_normalMapTex",
                                                          DataLine(VectorF(10.0f, 10.0f)),
                                                          DataLine(VectorF(-1.5f, 0.0f))));
    texSamplerName = ((TextureSampleNode*)(normalMap.get()))->GetSamplerUniformName();

    DNP finalNormal(new NormalizeNode(DataLine(DNP(new AddNode(DataLine(normalMap, 0), DataLine(waterNode, WaterNode::GetSurfaceNormalOutputIndex()))), 0)));

    DNP light(new LightingNode(DataLine(DNP(new VertexOutputNode(RC::RC_VERTEX_OUT_4, 3)), 0),
                               DataLine(DNP(new ObjectNormalToWorldNormalCalcNode(DataLine(finalNormal, 0))), 0),
                               DataLine(Vector3f(-1, -1, -1).Normalized()),
                               DataLine(0.35f), DataLine(0.65f), DataLine(3.0f), DataLine(256.0f)));

    DNP finalColor(new MultiplyNode(DataLine(light, 0), DataLine(Vector3f(0.275f, 0.275f, 1.0f))));

    channels[RC::RC_Color] = DataLine(DNP(new MultiplyNode(DataLine(light, 0),
                                                           DataLine(Vector3f(0.275f, 0.275f, 1.0f)))), 0);
    channels[RC::RC_ScreenVertexPosition] = DataLine(DNP(new ObjectPosToScreenPosCalcNode(DataLine(waterNode, WaterNode::GetVertexPosOutputIndex()))), 0);


    #pragma endregion


    //Post-processing.
    typedef PostProcessEffect::PpePtr PpePtr;
    ppcChain.insert(ppcChain.end(), PpePtr(new FogEffect(DataLine(VectorF(1.5f)), DataLine(VectorF(Vector3f(1.0f, 1.0f, 1.0f))))));


    //Final render.
    //TODO: Finish.
    finalScreenMatChannels[RC::RC_Color] = DataLine(DataNodePtr(new TextureSampleNode("u_finalRenderSample")), TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    UniformDictionary uniformDict;
    finalScreenMat = ShaderGenerator::GenerateMaterial(finalScreenMatChannels, uniformDict, RenderingModes::RM_Opaque, false, LightSettings(false));
    if (finalScreenMat->HasError())
    {
        std::cout << "final screen material creation error: " << finalScreenMat->GetErrorMsg() << "\n";
        Pause();
        EndWorld();
        return;
    }

    finalScreenQuad = new DrawingQuad();
    finalScreenQuad->GetMesh().Uniforms = uniformDict;
}
void OpenGLTestWorld::InitializeObjects(void)
{
    const unsigned int size = 300;

    water = new Water(size, Vector3f(0.0f, 0.0f, 0.0f), Vector3f(2.0f, 2.0f, 2.0f),
                      OptionalValue<Water::RippleWaterCreationArgs>(Water::RippleWaterCreationArgs(maxRipples)),
                      OptionalValue<Water::DirectionalWaterCreationArgs>(Water::DirectionalWaterCreationArgs(maxFlows)),
                      OptionalValue<Water::SeedmapWaterCreationArgs>(),
                      RenderingModes::RM_Opaque, true, LightSettings(false), channels);
    if (water->HasError())
    {
        std::cout << "Error creating water: " << water->GetErrorMessage() << "\n";
        Pause();
        EndWorld();
        return;
    }
    water->GetTransform().IncrementPosition(Vector3f(0.0f, 0.0f, -10.0f));

    const Material * waterMat = water->GetMaterial();
    water->GetMesh().Uniforms.TextureUniforms[texSamplerName] =
        UniformSamplerValue(&myTex, texSamplerName,
                            waterMat->GetUniforms(RenderPasses::BaseComponents).FindUniform(texSamplerName, waterMat->GetUniforms(RenderPasses::BaseComponents).TextureUniforms).Loc);

    water->AddFlow(Water::DirectionalWaterArgs(Vector2f(2.0f, 0.0f), 10.0f, 50.0f));


    ppc = new PostProcessChain(ppcChain, windowSize.x, windowSize.y, manager);
    if (ppc->HasError())
    {
        std::cout << "Error creating post-process chain: " << ppc->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
}


OpenGLTestWorld::OpenGLTestWorld(void)
: SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(24, 0, 0, 3, 3)),
  water(0), ppc(0), finalScreenQuad(0), finalScreenMat(0)
{
}
void OpenGLTestWorld::InitializeWorld(void)
{
	SFMLOpenGLWorld::InitializeWorld();
	if (IsGameOver()) return;
	

    Input.AddBoolInput(666, BoolInputPtr((BoolInput*)new MouseBoolInput(sf::Mouse::Button::Left, BoolInput::ValueStates::JustPressed)));


	GetWindow()->setVerticalSyncEnabled(true);
	GetWindow()->setMouseCursorVisible(true);

    InitializeTextures();
    InitializeMaterials();
    InitializeObjects();


    Vector3f pos(2.0f, 2.0f, 10.0f);
    cam.SetPosition(pos);
    cam.SetRotation(-pos, Vector3f(0.0f, 0.0f, 1.0f), false);
    cam.Window = GetWindow();
    cam.Info.FOV = ToRadian(55.0f);
    cam.Info.zFar = 500.0f;
    cam.Info.zNear = 1.0f;
    cam.Info.Width = windowSize.x;
    cam.Info.Height = windowSize.y;
    cam.SetMoveSpeed(60.0f);
    cam.SetRotSpeed(0.25f);
}

OpenGLTestWorld::~OpenGLTestWorld(void)
{
    DeleteAndSetToNull(water);
    DeleteAndSetToNull(ppc);
    DeleteAndSetToNull(finalScreenQuad);
    DeleteAndSetToNull(finalScreenMat);
}
void OpenGLTestWorld::OnWorldEnd(void)
{
    DeleteAndSetToNull(water);
    DeleteAndSetToNull(ppc);
    DeleteAndSetToNull(finalScreenQuad);
    DeleteAndSetToNull(finalScreenMat);
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

    if (Input.GetBoolInputValue(666))
        water->AddRipple(Water::RippleWaterArgs(cam.GetPosition(), 5000.0f, 10.0f, 120.0f, 1.0f));
}

void OpenGLTestWorld::RenderWorldGeometry(const RenderInfo & info)
{
    //Render the world into a render target.

    manager[worldRenderID]->EnableDrawingInto();
    ScreenClearer().ClearScreen();

    if (!water->Render(info))
    {
        std::cout << "Error rendering world geometry: " << water->GetErrorMessage() << ".\n";
        Pause();
        EndWorld();
        return;
    }

    manager[worldRenderID]->DisableDrawingInto(windowSize.x, windowSize.y);

    std::string err = GetCurrentRenderingError();
    if (!err.empty())
    {
        std::cout << "Error rendering world geometry: " << err << "\n";
        Pause();
        EndWorld();
        return;
    }

    //Render post-process effects on top of the world.
    if (!ppc->RenderChain(this, cam.Info, manager[worldRenderID]))
    {
        std::cout << "Error rendering post-process chain: " << ppc->GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
    
    ScreenClearer().ClearScreen();
    //Render the final image.
    Camera cam;
    TransformObject trans;
    Matrix4f identity;
    identity.SetAsIdentity();
    RenderTarget * finalRend = ppc->GetFinalRender();
    if (finalRend == 0) finalRend = manager[worldRenderID];
    finalScreenQuad->GetMesh().Uniforms.TextureUniforms["u_finalRenderSample"].Texture.SetData(finalRend->GetColorTexture());
    if (!finalScreenQuad->Render(RenderPasses::BaseComponents, RenderInfo(this, &cam, &trans, &identity, &identity, &identity), *finalScreenMat))
    {
        std::cout << "Error rendering final screen output: " << finalScreenMat->GetErrorMsg() << "\n";
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
    if (!manager.ResizeTarget(worldRenderID, newW, newH))
    {
        std::cout << "Error resizing world render target: " << manager.GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
    if (!ppc->ResizeRenderTargets(newW, newH))
    {
        std::cout << "Error resizing PPC render target: " << manager.GetError() << "\n";
        Pause();
        EndWorld();
        return;
    }
}