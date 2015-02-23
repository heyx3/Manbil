#pragma once

#include "../../SFMLOpenGLWorld.h"
#include "../../Rendering/Rendering.hpp"
#include "PlanetSimCamera.h"
#include "PlanetSimWorldGen.h"


//Runs the PlanetSim project.
class PlanetSimWorld : public SFMLOpenGLWorld
{
public:

    PlanetSimWorld(void);

protected:
    
    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) override;

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;

    virtual void OnWindowResized(unsigned int newW, unsigned int newH) override;

private:

    void RenderWorld(float elapsedSeconds, RenderInfo& camInfo);
    void PrintError(std::string error);

    Material* planetMat;
    UniformDictionary planetParams;
    MTexture2D planetHeightTex;
    MTexture3D planetTex3D;

    WorldData* world;
    PlanetSimCamera cam;

    Vector2u windowSize;
};