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
    //Removes the chunk with the given index if it exists.
    //Returns whether or not it existed.
    bool DestroyChunk(Vector3i index)
    {
        auto location = chunks.find(index);
        if (location == chunks.end()) return false;

        delete location->second;
        chunks.erase(location);
        return true;
    }


    template<typename Func>
    //"Func" must have the signature "bool Func(Vector3i chunkIndex, VoxelChunk * chunk)".
    //"Func" returns whether to exit "DoToEveryChunk" after calling it.
    //Calls "todo" on every valid local chunk index between "start" and "end", inclusive.
    //Assume the "chunk" parameter in Func is never 0.
    //Returns whether or not "todo" ever returned "true".
    bool DoToEveryChunkPredicate(Func todo, Vector3i start = Vector3i(0, 0, 0), Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1))
    {
        int xStart = BasicMath::Min(start.x, end.x),
            yStart = BasicMath::Min(start.y, end.y),
            zStart = BasicMath::Min(start.z, end.z);
        int xEnd = BasicMath::Max(start.x, end.x),
            yEnd = BasicMath::Max(start.y, end.y),
            zEnd = BasicMath::Max(start.z, end.z);

        Vector3i loc;
        for (loc.z = zStart; loc.z <= zEnd; ++loc.z)
            for (loc.y = yStart; loc.y <= yEnd; ++loc.y)
                for (loc.x = xStart; loc.x <= xEnd; ++loc.x)
                {
                    VoxelChunk * cnk = GetChunk(loc);
                    if (cnk != 0 && todo(loc, cnk))
                        return true;
                }
        return false;
    }
    template<typename Func>
    //"Func" must have the signature "void Func(Vector3i chunkIndex, VoxelChunk * chunk)".
    //Calls "todo" on every valid local chunk index between "start" and "end", inclusive.
    //Assume the "chunk" parameter in Func is never 0.
    void DoToEveryChunk(Func todo, Vector3i start = Vector3i(0, 0, 0), Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1))
    {
        int xStart = BasicMath::Min(start.x, end.x),
            yStart = BasicMath::Min(start.y, end.y),
            zStart = BasicMath::Min(start.z, end.z);
        int xEnd = BasicMath::Max(start.x, end.x),
            yEnd = BasicMath::Max(start.y, end.y),
            zEnd = BasicMath::Max(start.z, end.z);

        Vector3i loc;
        for (loc.z = zStart; loc.z <= zEnd; ++loc.z)
            for (loc.y = yStart; loc.y <= yEnd; ++loc.y)
                for (loc.x = xStart; loc.x <= xEnd; ++loc.x)
                {
                    VoxelChunk * cnk = GetChunk(loc);
                    if (cnk != 0) todo(loc, cnk);
                }
    }
    template<typename Func>
    //"Func" must have the signature "bool Func(Vector3i chunkIndex, VoxelChunk * chunk)".
    //"Func" returns whether to exit "DoToEveryChunkPredicate" after calling it.
    //Calls "todo" on every valid local chunk index between "start" and "end", inclusive.
    //Assume the "chunk" parameter in Func is never 0.
    //Returns whether or not "todo" ever returned "true".
    bool DoToEveryChunkPredicate(Func todo, Vector3i start = Vector3i(0, 0, 0), Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1)) const
    {
        int xStart = BasicMath::Min(start.x, end.x),
            yStart = BasicMath::Min(start.y, end.y),
            zStart = BasicMath::Min(start.z, end.z);
        int xEnd = BasicMath::Max(start.x, end.x),
            yEnd = BasicMath::Max(start.y, end.y),
            zEnd = BasicMath::Max(start.z, end.z);

        Vector3i loc;
        for (loc.z = zStart; loc.z <= zEnd; ++loc.z)
            for (loc.y = yStart; loc.y <= yEnd; ++loc.y)
                for (loc.x = xStart; loc.x <= xEnd; ++loc.x)
                {
                    VoxelChunk * cnk = GetChunk(loc);
                    if (cnk != 0 && todo(loc, cnk))
                        return true;
                }
        return false;
    }
    template<typename Func>
    //"Func" must have the signature "void Func(Vector3i chunkIndex, VoxelChunk * chunk)".
    //Calls "todo" on every valid local chunk index between "start" and "end", inclusive.
    //Assume the "chunk" parameter in Func is never 0.
    bool DoToEveryChunk(Func todo, Vector3i start = Vector3i(0, 0, 0), Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1)) const
    {
        int xStart = BasicMath::Min(start.x, end.x),
            yStart = BasicMath::Min(start.y, end.y),
            zStart = BasicMath::Min(start.z, end.z);
        int xEnd = BasicMath::Max(start.x, end.x),
            yEnd = BasicMath::Max(start.y, end.y),
            zEnd = BasicMath::Max(start.z, end.z);

        Vector3i loc;
        for (loc.z = zStart; loc.z <= zEnd; ++loc.z)
            for (loc.y = yStart; loc.y <= yEnd; ++loc.y)
                for (loc.x = xStart; loc.x <= xEnd; ++loc.x)
                {
                    VoxelChunk * cnk = GetChunk(loc);
                    if (cnk != 0) todo(loc, cnk);
                }
    }




private:

    std::unordered_map<Vector3i, VoxelChunk*, Vector3i> chunks;
};