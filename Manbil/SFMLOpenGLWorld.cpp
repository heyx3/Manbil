#include "SFMLOpenGLWorld.h"

#include <iostream>

#include "OpenGLIncludes.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

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
        std::string err = TextRenderer::InitializeSystem();
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
    if (drawQuad)
    {
        DrawingQuad::DestroyQuadData();
    }
    if (textRend)
    {
        TextRenderer::DestroySystem();
    }
}


SFMLOpenGLWorld::SFMLOpenGLWorld(int windowWidth, int windowHeight, sf::ContextSettings settings)
    : SFMLWorld(windowWidth, windowHeight, settings), TextRender(RenderTargets, Textures)
{
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