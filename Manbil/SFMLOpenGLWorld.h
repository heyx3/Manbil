#pragma once

#include "SFMLWorld.h"
#include "Rendering/GUI/FreeTypeHandler.h"

//A world that uses SFML and OpenGL.
class SFMLOpenGLWorld : public SFMLWorld
{
public:

    FreeTypeHandler FontManager;

    SFMLOpenGLWorld(int windowWidth, int windowHeight, sf::ContextSettings settings = sf::ContextSettings());
	~SFMLOpenGLWorld(void) { }

protected:

	virtual void InitializeWorld(void) override;
	virtual void OnInitializeError(std::string errorMsg) { fprintf(stderr, "Initialization error: '%s'\n", errorMsg.c_str()); }

    virtual void RenderWorld(float elapsedSeconds) override;

    //First, render all the OpenGL stuff.
    virtual void RenderOpenGL(float elapsedSeconds) = 0;
};