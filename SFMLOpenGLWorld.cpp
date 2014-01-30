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
}