#pragma once

#include "../SFMLOpenGLWorld.h"
#include "VoxelChunk.h"
#include "../MovingCamera.h"
#include "../RenderingState.h"
#include "../Material.h"
#include "../Mesh.h"
#include "../Math/Higher Math/Lighting.h"


//A world full of voxels.
class VoxelWorld : public SFMLOpenGLWorld
{
public:

    VoxelWorld(void);


protected:

    virtual void InitializeWorld(void) override;
    virtual void OnWorldEnd(void) override;

    virtual void UpdateWorld(float elapsedSeconds) override;
    virtual void RenderOpenGL(float elapsedSeconds) override;

    virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight);


private:

    VoxelChunk chunk;
    MovingCamera cam;

    RenderingState renderState;
    DirectionalLight light;
    
    Material * voxelMat;
    Mesh voxelMesh;
};