#pragma once

#include <vector>
#include "../Math/Shapes/Boxes.h"
#include "../Math/Fake2DArray.h"
#include "../Math/Fake3DArray.h"

class Shape;

//Represents a grid of voxels.
//There are three different coordinate spaces used:
//-World Space: regular world space.
//-World Chunk Space: a scaled version of world space where every unit represents one voxel length.
//-Local Chunk Space: a translated World Chunk Space for a specific VoxelChunk instance --
//    { 0, 0, 0 } is the chunk's inclusive min corner, and
//    { ChunkSize, ChunkSize, ChunkSize } is the chunk's exclusive max corner.
class VoxelChunk
{
public:

    //The width/height/depth of the chunk, in voxels.
    static const int ChunkSize;
    //The "ChunkSize" constant as a float so no run-time conversion is needed.
    //TODO: Would a run-time conversion actually be needed anyway? Size is a compile-time constant.
    static const float ChunkSizeF;

    //The width/height/depth of a voxel, in world units.
    static const int VoxelSize;
    //The "VoxelSize" constant as a float so no run-time conversion is needed.
    //TODO: Would a run-time conversion actually be needed anyway? Size is a compile-time constant.
    static const float VoxelSizeF;


    //The world position of the top-left-back corner.
    Vector3i MinCorner;


    VoxelChunk(Vector3i minCorner) : MinCorner(minCorner), nSolidVoxels(0), voxels(ChunkSize, ChunkSize, ChunkSize, false) { }


    //Converts world chunk coordinates to world coordinates.
    static Vector3f ToWorldSpace(Vector3i chunkCoord) { return ToWorldSpace(Vector3f(chunkCoord.x, chunkCoord.y, chunkCoord.z)); }
    //Converts world chunk coordinates to world coordinates.
    static Vector3f ToWorldSpace(Vector3f chunkCoord) { return chunkCoord * VoxelSizeF; }

    //Converts local chunk coordinates to world Coordinates.
    Vector3f LocalToWorldSpace(Vector3i chunkCoord) const { return LocalToWorldSpace(Vector3f(chunkCoord.x, chunkCoord.y, chunkCoord.z)); }
    //Converts local chunk coordinates to world Coordinates.
    Vector3f LocalToWorldSpace(Vector3f chunkCoord) const { return (chunkCoord + Vector3f(MinCorner.x, MinCorner.y, MinCorner.z)) * VoxelSizeF; }

    //Converts world coordinates to world Chunk coordinates.
    static Vector3f ToChunkSpace(Vector3f worldSpace) { return worldSpace / VoxelSizeF; }
    //Converts world coordinates to local Chunk coordinates.
    Vector3f ToLocalChunkSpace(Vector3f worldSpace) const { return ToChunkSpace(worldSpace) - Vector3f(MinCorner.x, MinCorner.y, MinCorner.z); }
    
    //Converts world coordinates to the world chunk coordinate of the nearest voxel.
    static Vector3i ToVoxelIndex(Vector3f worldSpace) { return (worldSpace / VoxelSizeF).Floored(); }
    //Converts world coordinates to the coordinate of the nearest local voxel.
    Vector3i ToLocalVoxelIndex(Vector3f worldSpace) const { return ToVoxelIndex(worldSpace) - MinCorner; }

    //Clamps the given Local-Chunk-Space coordinate to be inside this Chunk.
    Vector3i Clamp(Vector3i inV) const { return inV.Clamp(0, ChunkSize); }
    //Clamps the given Local-Chunk-Space coordinate to be inside this Chunk.
    Vector3f Clamp(Vector3f inV) const { return inV.Clamp(0.0f, ChunkSizeF); }


    //'location' is in world coordinates.
    bool GetVoxelWorld(Vector3f location) const { return GetVoxelLocal((location / VoxelSizeF).Floored() - MinCorner); }
    //'location' is in world coordinates.
    bool ToggleVoxelWorld(Vector3f location) { return ToggleVoxelLocal((location / VoxelSizeF).Floored() - MinCorner); }
    //'location' is in world coordinates.
    void SetVoxelWorld(Vector3f location, bool value) { SetVoxelLocal((location / VoxelSizeF).Floored() - MinCorner, value); }

    //'location' is in chunk coordinates.
    bool GetVoxelLocal(Vector3i location) const { return voxels[location]; }
    //'location' is in chunk coordinates. Returns the new value of the voxel.
    bool ToggleVoxelLocal(Vector3i location)
    {
        bool old = voxels[location];
        voxels[location] = !voxels[location];
        if (old) nSolidVoxels -= 1;
        else nSolidVoxels += 1;
        return !old;
    }
    //'location' is in chunk coordinates.
    void SetVoxelLocal(Vector3i location, bool value)
    {
        bool old = voxels[location];
        if (old != value)
        {
            if (value) nSolidVoxels += 1;
            else nSolidVoxels -= 1;

            voxels[location] = value;
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



    //Returns the index of the first voxel that is hit by the given ray (in Local Chunk space),
    //    or Vector3i(-1, -1, -1) if nothing was hit.
    Vector3i CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist = ChunkSizeF + ChunkSizeF) const;

    //Builds the world-space triangles/indices for this chunk, given all surrounding chunks.
    //Any of the surrounding chunks passed in may have values of 0 if they don't exist.
    void BuildTriangles(std::vector<Vector3f> & vertices, std::vector<unsigned int> & indices,
                        const VoxelChunk * beforeMinX, const VoxelChunk * afterMaxX,
                        const VoxelChunk * beforeMinY, const VoxelChunk * afterMaxY,
                        const VoxelChunk * beforeMinZ, const VoxelChunk * afterMaxZ) const;


    //Gets the voxels.
    const Fake3DArray<bool> & GetVoxels(void) const { return voxels; }

    //Gets the number of solid voxels in this chunk.
    unsigned int GetNumbSolidVoxels(void) const { return nSolidVoxels; }
    //Gets whether or not this chunk is empty.
    bool IsEmpty(void) const { return nSolidVoxels == 0; }
    //Gets whether or not this chunk is full.
    bool IsFull(void) const { return nSolidVoxels == (ChunkSize * ChunkSize * ChunkSize); }


    //Gets the bounds around this chunk.
    Box3D GetBounds(void) const { return Box3D(MinCorner.x, MinCorner.y, MinCorner.z, Vector3f(ChunkSizeF, ChunkSizeF, ChunkSizeF)); }


private:

    Fake3DArray<bool> voxels;
    unsigned int nSolidVoxels;
};