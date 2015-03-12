#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "../Rendering/Rendering.hpp"

#include "../Input/MovingCamera.h"
#include "../Rendering/Rendering.hpp"
#include "../Rendering/Water/Water.h"


//A sample world that demonstrates the Water system, which is an example of how to take advantage of
//    the "DataNode" system.
//The Water class has two types of ripples: "directional flow" (waves along a horizontal direction),
//    and "circular ripples" (ripples moving outward from a point).
//The exact number of flows and ripples is specified when the water is created (although you can
//    effectively destroy a wave or ripple by setting its amplitude to 0).
//In this world, the directional ripples are set when the water is created, and the circular ripples
//    are created by the player when he left-clicks.
class WaterWorld : public SFMLOpenGLWorld
{
public:
    WaterWorld(void);
    
protected:

    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH) override;
    virtual std::string GetWindowTitle(void) override;
    virtual sf::Uint32 GetSFStyleFlags(void) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;

private:

    Vector2u windowSize;
    MovingCamera gameCam;

    //Used for generating random water ripples.
    FastRand fastRand;


    //This "Water" object contains its own mesh and uniform collection,
    //    so we don't need to manage that ourselves.
    Water* waterObj;
    Material* waterMat;
    MTexture2D normalMap1, normalMap2;


    //Display a static skybox so that the water has something to reflect.

    Material* skyboxMat;
    UniformDictionary skyboxParams;
    MTextureCubemap skyboxTex;
    Mesh skyboxMesh;

    
    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);
};