#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../Input/MovingCamera.h"


class Terrain;


//A sample world that shows off the Terrain class.
//Displays different levels of detail in the terrain that you can cycle through
//    with the left/right arrow keys.
//The terrain's heightmap is created with the Noise Generation system.
class TerrainWorld : public SFMLOpenGLWorld
{
public:

    TerrainWorld(void);
    virtual ~TerrainWorld(void);


protected:

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

    //Generates the terrain submesh for the given detail level.
    void GenerateTerrainLOD(const Terrain& terr, unsigned int lodLevel);

    void InitializeTextures(void);
    void InitializeMaterials(void);
    void InitializeObjects(void);

    //Renders the world geometry using the given rendering info.
    void RenderWorldGeometry(const RenderInfo& info);

    
    //Each Level of Detail of the terrain is stored as a different MeshData instance
    //    inside a single mesh instance.
    Mesh terrMesh;

    Material* terrMat;
    UniformDictionary terrParams;
    MTexture2D terrTex;

    Vector2u windowSize;

    MovingCamera cam;
};