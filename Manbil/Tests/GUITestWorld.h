#pragma once

#include "../SFMLOpenGLWorld.h"
#include "../Rendering/Helper Classes/DrawingQuad.h"


class GUITestWorld : public SFMLOpenGLWorld
{
public:

    static Vector2i WindowSize;

    GUITestWorld(void)
        : SFMLOpenGLWorld(WindowSize.x, WindowSize.y, sf::ContextSettings(24, 0, 0, 3, 3)),
          quad(0), quadMat(0)
    {
    }
    virtual ~GUITestWorld(void) { DestroyMyStuff(false); }


protected:

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) { DestroyMyStuff(true); }
    
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;


private:

    //If there is an error, prints 'errorIntro + ": " + errorMsg', ends the world, and returns false.
    bool ReactToError(bool isEverythingOK, std::string errorIntro, std::string errorMsg);

    void DestroyMyStuff(bool destroyStatics);


    DrawingQuad * quad;
    UniformDictionary quadParams;
    Material * quadMat;
};