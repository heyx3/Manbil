#include "SFMLOpenGLWorld.h"

#include "OpenGLIncludes.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

void SFMLOpenGLWorld::InitializeWorld(void)
{
	//Set up GLEW.
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		char * cs = (char*)(glewGetErrorString(res));
		OnInitializeError(std::string(cs));
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	GetWindow()->setVerticalSyncEnabled(true);

    //GetWindow()->setActive();
}

void SFMLOpenGLWorld::RenderWorld(float elapsedSeconds)
{
    RenderOpenGL(elapsedSeconds);
    //TODO: Saving ALL states is needlessly expensive. Change to only save important rendering states.
    //GetWindow()->pushGLStates();
    RenderSFML(elapsedSeconds);
    //GetWindow()->popGLStates();
    GetWindow()->display();
}