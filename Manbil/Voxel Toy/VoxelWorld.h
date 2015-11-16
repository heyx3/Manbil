#pragma once

#include "../Game Loop/SFMLOpenGLWorld.h"
#include "VoxelChunkManager.h"
#include "ChunkMesh.h"
#include "VoxelPlayer.h"
#include "../Rendering/Basic Rendering/RenderingState.h"
#include "../Rendering/Basic Rendering/Material.h"
#include "../Rendering/Basic Rendering/Mesh.h"
#include "../Rendering/Primitives/DrawingQuad.h"
#include "../Rendering/Textures/RenderTargetManager.h"
#include "../Rendering/Textures/MTexture2D.h"

class VoxelWorldPPC;


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

    VoxelChunk* GetCreateChunk(Vector3i location)
    {
        if (manager.DoesChunkExist(location))
        {
            return manager.GetChunk(location);
        }

        VoxelChunk* chnk = manager.GetCreateChunk(location);
        chunkMeshes[location] = new ChunkMesh(manager, location, chnk);
        return manager.GetChunk(location);
    }
    bool DestroyChunk(Vector3i location)
    {
        if (manager.DestroyChunk(location))
        {
            auto loc = chunkMeshes.find(location);

            delete loc->second;
            chunkMeshes.erase(loc);

            return true;
        }
        return false;
    }
    const Mesh& GetMesh(Vector3i chunkIndex) { return chunkMeshes[chunkIndex]->GetMesh(); }

    void SetUpVoxels(void);


    VoxelChunkManager manager;
    std::unordered_map<Vector3i, ChunkMesh*, Vector3i> chunkMeshes;

    VoxelPlayer player;

    RenderingState renderState;


    Material* voxelMat;
    UniformDictionary voxelParams;
    MTexture2D voxelTex;

    Material* voxelHighlightMat;
    Mesh voxelHighlightMesh;
    UniformDictionary voxelHighlightParams;

    RenderTarget* worldRenderTarget;
    MTexture2D worldRenderTargetColorTex, worldRenderTargetDepthTex;
    VoxelWorldPPC* postProcessing;

    Material* finalWorldRenderMat;
    UniformDictionary finalWorldRenderParams;
};