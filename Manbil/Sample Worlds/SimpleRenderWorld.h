#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../Input/MovingCamera.h"


class SimpleRenderWorld : public SFMLOpenGLWorld
{
public:

    SimpleRenderWorld(void);
    
protected:

    virtual sf::VideoMode GetModeToUse(unsigned int windowW, unsigned int windowH) override;
    virtual std::string GetWindowTitle(void) override;
    virtual sf::Uint32 GetSFStyleFlags(void) override;
    virtual sf::ContextSettings GenerateContext(void) override;


    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) override;

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;

private:

    //Keep all the material stuff in its own function.
    void SetUpMaterial(void);


    Vector2u windowSize;


    //A basic movable camera.
    MovingCamera gameCam;

    //A material to draw our object with.
    std::unique_ptr<Material> objMat;
    UniformDictionary objMatParams;

    //A mesh to be drawn.
    std::unique_ptr<Mesh> objMesh;

	//The position, rotation, and scale of the mesh.
	Transform objTr;
};
