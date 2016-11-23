#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../Input/MovingCamera.h"


//A simple world that tests the Terrain class.
class DataNodeRenderWorld : public SFMLOpenGLWorld
{
public:

    DataNodeRenderWorld(void);


protected:

    virtual std::string GetWindowTitle(void) { return "DataNodeRenderWorld"; }
    virtual sf::ContextSettings GenerateContext(void) override;

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newW, unsigned int newH) override;

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;


private:

    //If the given test value is false, the following is done:
    //  1) Prints the given error, then pauses for the user to acknowledge.
    //  2) Ends the world.
    //  3) Returns false.
    //Otherwise, returns true.
    bool Assert(bool test, std::string errorIntro, const std::string& error);


    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);

    //Renders the world geometry using the given rendering info.
    void RenderWorldGeometry(const RenderInfo& info);


    std::unique_ptr<Material> objMat;
    std::unique_ptr<Mesh> objMesh;
	Transform objTransform;
    UniformDictionary objMatParams;
    MTexture2D objTex;

    Vector2u windowSize;

    MovingCamera cam;
};