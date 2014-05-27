#pragma once

#include <unordered_map>
#include "VoxelChunk.h"


//Manages a bunch of chunks.
//There are two different coordinate spaces used:
//-World Space: regular world space.
//-World Chunk Space: a scaled version of world space where every unit represents one voxel length.
class VoxelChunkManager
{
public:

    typedef std::unordered_map<Vector3i, VoxelChunk*, Vector3i> VoxelChunkDictionary;


    //Converts the given world position to a chunk index value (in between actual indices).
    static Vector3f ToChunkIndexF(Vector3f worldPos) { return (worldPos / (VoxelChunk::VoxelSizeF * VoxelChunk::ChunkSizeF)); }
    //Converts the given world position to a chunk index value.
    static Vector3i ToChunkIndex(Vector3f worldPos) { return (worldPos / (VoxelChunk::VoxelSizeF * VoxelChunk::ChunkSizeF)).Floored(); }
    //Converts the given world position to a chunk index value.
    static Vector3i ToChunkIndex(Vector3i worldPos) { return worldPos / (VoxelChunk::VoxelSize * VoxelChunk::ChunkSize); }
    //Returns the chunk index of the given chunk.
    static Vector3i GetChunkIndex(const VoxelChunk * chunk) { return ToChunkIndex(chunk->MinCorner + Vector3i(1, 1, 1)); }


    struct RayCastResult
    {
    public:
        VoxelChunk * Chunk;
        VoxelChunk::VoxelRayHit ChunkRayCastResult;
        RayCastResult(VoxelChunk * chunk = 0, VoxelChunk::VoxelRayHit chunkRayCastResult = VoxelChunk::VoxelRayHit())
            : Chunk(chunk), ChunkRayCastResult(chunkRayCastResult) { }
    };
    //Casts a ray and returns the voxel that was hit.
    //If nothing was hit, returns RayCastResult(0, Vector3i(-1, -1, -1)).
    //The ray cast fails if it runs into a missing chunk.
    RayCastResult CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist = 999999.0f) const;


    const VoxelChunkDictionary & GetAllChunks(void) const { return chunks; }


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
            chunks[index] = new VoxelChunk(index * VoxelChunk::ChunkSize * VoxelChunk::VoxelSize);
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


    //The location of a voxel in this chunk manager.
    struct VoxelLocation
    {
    public:
        VoxelChunk * Chunk;
        Vector3i LocalIndex;
        VoxelLocation(VoxelChunk * chunk = 0, Vector3i localIndex = Vector3i()) : Chunk(chunk), LocalIndex(localIndex) { }
    };
    //Gets the voxel equal to the given voxel offset by the given amount.
    VoxelLocation GetOffset(VoxelLocation voxel, Vector3i offset) const;


    template<typename Func>
    //"Func" must have the signature "void Func(Vector3i chunkIndex, VoxelChunk * chunk)".
    //Calls "todo" on every chunk owned by this manager.
    void DoToEveryChunk(Func todo)
    {
        for (auto iterator = chunks.begin(); iterator != chunks.end(); ++iterator)
            todo(iterator->first, iterator->second);
    }
    template<typename Func>
    //"Func" must have the signature "bool Func(Vector3i chunkIndex, VoxelChunk * chunk)".
    //Calls "todo" on every chunk owned by this manager.
    //Once a "todo" call returns true, this function immediately exits.
    //Returns whether a "todo" call returned true, causing this to immediately exit.
    void DoToEveryChunkPredicate(Func todo)
    {
        for (auto iterator = chunks.begin(); iterator != chunks.end(); ++iterator)
            if (todo(iterator->first, iterator->second))
                return true;
        return false;
    }

    template<typename Func>
    //"Func" must have the signature "bool Func(Vector3i chunkIndex, VoxelChunk * chunk)".
    //"Func" returns whether to exit "DoToEveryChunk" after calling it.
    //Calls "todo" on every valid local chunk index between "start" and "end", inclusive.
    //Assume the "chunk" parameter in Func is never 0.
    //Returns whether or not "todo" ever returned "true".
    bool DoToEveryChunkPredicate(Func todo, Vector3i start, Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1))
    {
        Vector3i sign(BasicMath::Sign(end.x - start.x), BasicMath::Sign(end.y - start.y), BasicMath::Sign(end.z - start.z));
        if (sign.x == 0) sign.x = 1;
        if (sign.y == 0) sign.y = 1;
        if (sign.z == 0) sign.z = 1;

        Vector3i loc;
        for (loc.z = start.z; (sign.z > 0 && loc.z <= end.z) || (sign.z < 0 && loc.z >= end.z); loc.z += sign.z)
            for (loc.y = start.y; (sign.y > 0 && loc.y <= end.y) || (sign.y < 0 && loc.y >= end.y); loc.y += sign.y)
                for (loc.x = start.x; (sign.x > 0 && loc.x <= end.x) || (sign.x < 0 && loc.x >= end.x); loc.x += sign.x)
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
    void DoToEveryChunk(Func todo, Vector3i start, Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1))
    {
        Vector3i sign(BasicMath::Sign(end.x - start.x), BasicMath::Sign(end.y - start.y), BasicMath::Sign(end.z - start.z));
        if (sign.x == 0) sign.x = 1;
        if (sign.y == 0) sign.y = 1;
        if (sign.z == 0) sign.z = 1;

        Vector3i loc;
        for (loc.z = start.z; (sign.z > 0 && loc.z <= end.z) || (sign.z < 0 && loc.z >= end.z); loc.z += sign.z)
            for (loc.y = start.y; (sign.y > 0 && loc.y <= end.y) || (sign.y < 0 && loc.y >= end.y); loc.y += sign.y)
                for (loc.x = start.x; (sign.x > 0 && loc.x <= end.x) || (sign.x < 0 && loc.x >= end.x); loc.x += sign.x)
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
    bool DoToEveryChunkPredicate(Func todo, Vector3i start, Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1)) const
    {
        Vector3i sign(BasicMath::Sign(end.x - start.x), BasicMath::Sign(end.y - start.y), BasicMath::Sign(end.z - start.z));
        if (sign.x == 0) sign.x = 1;
        if (sign.y == 0) sign.y = 1;
        if (sign.z == 0) sign.z = 1;

        Vector3i loc;
        for (loc.z = start.z; (sign.z > 0 && loc.z <= end.z) || (sign.z < 0 && loc.z >= end.z); loc.z += sign.z)
            for (loc.y = start.y; (sign.y > 0 && loc.y <= end.y) || (sign.y < 0 && loc.y >= end.y); loc.y += sign.y)
                for (loc.x = start.x; (sign.x > 0 && loc.x <= end.x) || (sign.x < 0 && loc.x >= end.x); loc.x += sign.x)
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
    bool DoToEveryChunk(Func todo, Vector3i start, Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1)) const
    {
        Vector3i sign(BasicMath::Sign(end.x - start.x), BasicMath::Sign(end.y - start.y), BasicMath::Sign(end.z - start.z));
        if (sign.x == 0) sign.x = 1;
        if (sign.y == 0) sign.y = 1;
        if (sign.z == 0) sign.z = 1;

        Vector3i loc;
        for (loc.z = start.z; (sign.z > 0 && loc.z <= end.z) || (sign.z < 0 && loc.z >= end.z); loc.z += sign.z)
            for (loc.y = start.y; (sign.y > 0 && loc.y <= end.y) || (sign.y < 0 && loc.y >= end.y); loc.y += sign.y)
                for (loc.x = start.x; (sign.x > 0 && loc.x <= end.x) || (sign.x < 0 && loc.x >= end.x); loc.x += sign.x)
                {
                    VoxelChunk * cnk = GetChunk(loc);
                    if (cnk != 0) todo(loc, cnk);
                }
    }


    //Gets whether ANY voxels with the majority of their volume inside the given Shape are solid.
    bool GetAnyVoxels(const Shape & shpe) const;
    //Gets whether ALL voxels with the majority of their volume inside the given Shape are solid.
    bool GetAllVoxels(const Shape & shpe) const;
    //Toggles all voxels with the majority of their volume inside the given Shape.
    void ToggleVoxels(const Shape & shpe);
    //Sets all voxels with the majority of their volume inside the given Shape.
    void SetVoxels(const Shape & shpe, bool value);


private:

    VoxelChunkDictionary chunks;
};