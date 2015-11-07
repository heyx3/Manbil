#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../Input/MovingCamera.h"



//A simple world that shows how to use the AssImp library.
//Use WASD/EQ and the mouse to move the camera.
//Use the arrow keys and right shift/ctrl to rotate the mesh.
//Use RFCV to move the mesh.
class AssetImporterWorld : public SFMLOpenGLWorld
{
public:

    AssetImporterWorld(void);
    virtual ~AssetImporterWorld(void);


protected:
    
    virtual std::string GetWindowTitle(void) { return "AssetImporterWorld"; }

    virtual void InitializeWorld(void) override;
    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;
    virtual void OnWorldEnd(void) override;

    virtual void OnInitializeError(std::string errorMsg) override;
    virtual void OnWindowResized(unsigned int newW, unsigned int newH) override;


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
    //Returns whether there was an error rendering.
    void RenderWorldGeometry(const RenderInfo& info);


    Material* objMat;
    Mesh objMesh;
    UniformDictionary objParams;
    MTexture2D objTex2;
    MTexture3D objTex3;

    Vector2u windowSize;

    MovingCamera cam;
};