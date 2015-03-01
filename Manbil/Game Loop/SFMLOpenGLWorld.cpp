#include "SFMLOpenGLWorld.h"

#include <iostream>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "../Rendering/Basic Rendering/OpenGLIncludes.h"
#include "../Rendering/GUI/TextRenderer.h"
#include "../Rendering/Data Nodes/DataNodes.hpp"
#include "../Rendering/Water/WaterRendering.h"
#include "../Rendering/PostProcessing/PostProcessData.h"


SFMLOpenGLWorld::SFMLOpenGLWorld(int windowWidth, int windowHeight, sf::ContextSettings settings)
    : SFMLWorld(windowWidth, windowHeight, settings)
{
}
SFMLOpenGLWorld::~SFMLOpenGLWorld(void)
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

void SFMLOpenGLWorld::OnInitializeError(std::string errorMsg)
{
    std::cout << "Initialization error: " << errorMsg.c_str() << "\n";
}