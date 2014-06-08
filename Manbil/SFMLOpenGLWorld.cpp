#include "SFMLOpenGLWorld.h"

#include <iostream>

#include "OpenGLIncludes.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>


SFMLOpenGLWorld::SFMLOpenGLWorld(int windowWidth, int windowHeight, sf::ContextSettings settings)
    : SFMLWorld(windowWidth, windowHeight, settings)
{
    if (FontManager.HasError())
    {
        std::cout << "Error initializing the world's font handler: " << FontManager.GetError();
        char dummy;
        std::cin >> dummy;
    }
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