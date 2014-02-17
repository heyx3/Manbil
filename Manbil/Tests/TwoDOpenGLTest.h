#pragma once

/*

#include "../SFMLOpenGLWorld.h"
#include "../Rendering/Rendering.hpp"
#include "../Rendering/Helper Classes/DrawingQuad.h"

//Tests 2D functionality for Manbil.
class TwoDOpenGLTest : public SFMLOpenGLWorld
{
public:

    TwoDOpenGLTest(void);
    ~TwoDOpenGLTest(void) { CleanUp(); }

protected:

    virtual void OnInitializeError(std::string errorMsg) override;

    virtual void InitializeWorld(void) override;

    virtual void OnWindowResized(unsigned int newW, unsigned int newH);

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;

    virtual void OnWorldEnd(void) override { CleanUp(); }

private:

    void CleanUp(void);


    Camera * cam;

    Material * backMat, * foreMat;
    DrawingQuad * foreQuad, * backQuad;
};

*/