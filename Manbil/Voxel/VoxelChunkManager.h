#pragma once

#include <unordered_map>
#include "VoxelChunk.h"


//Manages a bunch of chunks.
class VoxelChunkManager
{
public:

    //Converts the given world position to a chunk index value (in between actual indices).
    static Vector3f ToChunkIndexF(Vector3f worldPos) { return (worldPos / (VoxelChunk::VoxelSizeF * VoxelChunk::ChunkSizeF)); }
    //Converts the given world position to a chunk index value
    static Vector3i ToChunkIndex(Vector3f worldPos) { return (worldPos / (VoxelChunk::VoxelSizeF * VoxelChunk::ChunkSizeF)).Floored(); }


    struct RayCastResult
    {
    public:
        VoxelChunk * Chunk;
        Vector3i LocalChunkIndex;
        RayCastResult(VoxelChunk * chunk = 0, Vector3i localChunkIndex = Vector3i(-1, -1, -1)) : Chunk(chunk), LocalChunkIndex(localChunkIndex) { }
    };
    //Casts a ray and returns the voxel that was hit.
    //If nothing was hit, returns RayCastResult(0, Vector3i(-1, -1, -1)).
    //The ray cast fails if it runs into a missing chunk.
    RayCastResult CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist = 999999.0f) const;


    //Gets whether a chunk exists for the given chunk index.
    bool DoesChunkExist(Vector3i index) const
    {
        auto location = chunks.find(index);
        
        return location != chunks.end();
    }
    //Gets the chunk at the given index, or 0 if it doesn't exist.
    VoxelChunk * GetChunk(Vector3i index) const
    {
        auto location = chunks.find(index);

        return (location == chunks.end() ? 0 : location->second);
    }

    //Gets the chunk at the given index. If no chunk exists there,
    //    a new chunk is created and returned.
    VoxelChunk * GetCreateChunk(Vector3i index)
    {
        auto loc = chunks.find(index);
        if (loc == chunks.end())
        {
            chunks[index] = new VoxelChunk(index);
        }

        return chunks[index];
    }


private:

    std::unordered_map<Vector3i, VoxelChunk*, Vector3i> chunks;
};