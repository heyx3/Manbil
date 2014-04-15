#pragma once

#include "../SFMLOpenGLWorld.h"
#include "VoxelChunkManager.h"
#include "ChunkMesh.h"
#include "VoxelPlayer.h"
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

    VoxelChunk * GetCreateChunk(Vector3i location)
    {
        if (manager.DoesChunkExist(location))
            return manager.GetChunk(location);

        VoxelChunk * chnk = manager.GetCreateChunk(location);
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
    VertexIndexData GetMesh(Vector3i chunkIndex) { return chunkMeshes[chunkIndex]->GetVID(); }

    void SetUpVoxels(void);


    VoxelChunkManager manager;
    std::unordered_map<Vector3i, ChunkMesh*, Vector3i> chunkMeshes;

    VoxelPlayer player;

    RenderingState renderState;
    DirectionalLight light;
    
    Material * voxelMat;
    Mesh voxelMesh;
    unsigned int voxelTex;
};