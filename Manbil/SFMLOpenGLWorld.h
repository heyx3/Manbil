#pragma once

#include "SFMLWorld.h"

class TextRenderer;
class RenderTargetManager;


//A world that uses SFML and OpenGL.
class SFMLOpenGLWorld : public SFMLWorld
{
public:

    TextRenderer * TextRender;
    RenderTargetManager * RenderTargets;

    SFMLOpenGLWorld(int windowWidth, int windowHeight, sf::ContextSettings settings = sf::ContextSettings());
    virtual ~SFMLOpenGLWorld(void);


protected:


    //Starts up all static systems that should be started up, based on the booleans passed to this function.
    //Returns an error message, or an empty string if everything was fine.
    std::string InitializeStaticSystems(bool initializeOculusRift, bool initializeTextRenderer, bool initializeDrawingQuad);
    //Ends all static systems that should be ended, based on the booleans passed in to this function.
    void DestroyStaticSystems(bool destroyOculusRift, bool destroyTextRenderer, bool destroyDrawingQuad);


	virtual void InitializeWorld(void) override;
	virtual void OnInitializeError(std::string errorMsg) { fprintf(stderr, "Initialization error: '%s'\n", errorMsg.c_str()); }

    //By default, calls RenderOpenGL and then displays the window.
    virtual void RenderWorld(float elapsedSeconds) override;

    virtual void RenderOpenGL(float elapsedSeconds) = 0;
};