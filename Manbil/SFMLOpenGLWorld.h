#pragma once

#include "SFMLWorld.h"


//A world that uses SFML and OpenGL.
class SFMLOpenGLWorld : public SFMLWorld
{
public:

    //TextRenderer * TextRender;
    //RenderTargetManager * RenderTargets;

    SFMLOpenGLWorld(int windowWidth, int windowHeight,
                    sf::ContextSettings settings = sf::ContextSettings());
    virtual ~SFMLOpenGLWorld(void);


protected:

    //Starts up all static systems that should be started up,
    //    based on the booleans passed to this function.
    //Returns an error message, or an empty string if everything was fine.
    //TODO: Refactor this so it doesn't need to be explicitly called --
    //    use abstract functions for each system.
//    std::string InitializeStaticSystems(bool initializeOculusRift, bool initializeTextRenderer, bool initializeDrawingQuad);
    //Ends all static systems that should be ended, based on the booleans passed in to this function.
//    void DestroyStaticSystems(bool destroyOculusRift, bool destroyTextRenderer, bool destroyDrawingQuad);


	virtual void InitializeWorld(void) override;

    //Called if something goes wrong in this world's initialization.
    //Default behavior: Outputs the error message to the standard output scene.
    virtual void OnInitializeError(std::string errorMsg);

    //By default, calls RenderOpenGL and then displays the window.
    virtual void RenderWorld(float elapsedSeconds) override;

    virtual void RenderOpenGL(float elapsedSeconds) = 0;
};