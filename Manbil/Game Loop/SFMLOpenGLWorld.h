#pragma once

#include "SFMLWorld.h"


//An SFML world that uses native OpenGL.
class SFMLOpenGLWorld : public SFMLWorld
{
public:

    //TextRenderer * TextRender;
    //RenderTargetManager * RenderTargets;

    SFMLOpenGLWorld(int windowWidth, int windowHeight);
    virtual ~SFMLOpenGLWorld(void);


protected:

	virtual void InitializeWorld(void) override;

    //Called if something goes wrong in this world's initialization.
    //Default behavior: Outputs the error message to the standard output scene.
    virtual void OnInitializeError(std::string errorMsg);

    //By default, calls RenderOpenGL and then displays the window.
    virtual void RenderWorld(float elapsedSeconds) override;

    virtual void RenderOpenGL(float elapsedSeconds) = 0;
};