#include "SFMLOpenGLWorld.h"

#include <iostream>

#include "OpenGLIncludes.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Rendering/GUI/TextRenderer.h"

//The following #include statements are just for "InitializeStaticSystems".
#include "Oculus/OculusDevice.h"
#include "Rendering/GUI/TextRenderer.h"
#include "Rendering/Helper Classes/DrawingQuad.h"


std::string SFMLOpenGLWorld::InitializeStaticSystems(bool rift, bool textRenderer, bool drawingQuad)
{
    if (rift)
    {
        OculusDevice::InitializeSystem();
    }
    if (drawingQuad)
    {
        DrawingQuad::InitializeQuadData();
    }
    if (textRenderer)
    {
        if (FreeTypeHandler::Instance.HasError())
        {
            return FreeTypeHandler::Instance.GetError();
        }

        std::string err = TextRenderer::InitializeSystem(this);
        if (!err.empty()) return std::string() + "Error initializing TextRenderer system: " + err;
    }

    return "";
}
void SFMLOpenGLWorld::DestroyStaticSystems(bool rift, bool textRend, bool drawQuad)
{
    if (rift)
    {
        OculusDevice::DestroySystem();
    }
    if (textRend)
    {
        TextRenderer::DestroySystem();
    }
    if (drawQuad)
    {
        DrawingQuad::DestroyQuadData();
    }
}


SFMLOpenGLWorld::SFMLOpenGLWorld(int windowWidth, int windowHeight, sf::ContextSettings settings)
    : SFMLWorld(windowWidth, windowHeight, settings)
{
    RenderTargets = new RenderTargetManager();
    TextRender = new TextRenderer(*RenderTargets);
}
SFMLOpenGLWorld::~SFMLOpenGLWorld(void)
{
    delete TextRender;
    delete RenderTargets;
}

void SFMLOpenGLWorld::InitializeWorld(void)
{
	//Set up GLEW.
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		char * cs = (char*)(glewGetErrorString(res));
		OnInitializeError(std::string(cs));
	}

	GetWindow()->setVerticalSyncEnabled(true);
}

void SFMLOpenGLWorld::RenderWorld(float elapsedSeconds)
{
    RenderOpenGL(elapsedSeconds);
    GetWindow()->display();
}