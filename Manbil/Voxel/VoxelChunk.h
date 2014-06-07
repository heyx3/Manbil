#pragma once

#include <vector>
#include "../Math/Shapes/Boxes.h"
#include "../Math/Array2D.h"
#include "../Math/Array3D.h"
#include "../Math/Shapes/ThreeDShapes.h"
#include "../Vertices.h"



struct VoxelVertex
{
public:
    Vector3f Pos;
    //For each of the three axes, whether or not a quad should be made for that face.
    //A value of 0 indicates "no"; a value of 1 indicates "yes".
    Vector3f MinExists, MaxExists;
    VoxelVertex(Vector3f pos = Vector3f(), Vector3f minExists = Vector3f(1, 1, 1), Vector3f maxExists = Vector3f(1, 1, 1)) : Pos(pos), MinExists(minExists), MaxExists(maxExists) { }

    static VertexAttributes GetAttributeData(void) { return VertexAttributes(3, 3, 3, false, false, false); }
};


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
    static const unsigned int ChunkSize;
    //The "ChunkSize" constant as a float so no run-time conversion is needed.
    //TODO: Would a run-time conversion actually be needed anyway? Size is a compile-time constant.
    static const float ChunkSizeF;

    //The width/height/depth of a voxel, in world units.
    static const unsigned int VoxelSize;
    //The "VoxelSize" constant as a float so no run-time conversion is needed.
    //TODO: Would a run-time conversion actually be needed anyway? Size is a compile-time constant.
    static const float VoxelSizeF;

    //Local Chunk Space bounding values.
    struct MinMaxI { public: Vector3i Min, Max; };
    //Local Chunk Space bounding values.
    struct MinMaxF { public: Vector3f Min, Max; };



    //The world position of the top-left-back corner.
    Vector3i MinCorner;


    VoxelChunk(Vector3i minCorner) : MinCorner(minCorner), nSolidVoxels(0), voxels(ChunkSize, ChunkSize, ChunkSize, false) { }

    VoxelChunk(void); //Intentionally not implemented.
    VoxelChunk(const VoxelChunk & cpy); //Intentionally not implemented.


    //TODO: Move all these space conversions into their own class, except maybe the local chunk space ones.
    //Converters between World Space and World Chunk Space.

    //Converts world chunk coordinates to world coordinates.
    static Vector3f ToWorldSpace(Vector3i chunkCoord) { return ToWorldSpace(Vector3f((float)chunkCoord.x, (float)chunkCoord.y, (float)chunkCoord.z)); }
    //Converts world chunk coordinates to world coordinates.
    static Vector3f ToWorldSpace(Vector3f chunkCoord) { return chunkCoord * VoxelSizeF; }

    //Converts world coordinates to world Chunk coordinates.
    static Vector3f ToWorldChunkSpace(Vector3f worldSpace) { return worldSpace / VoxelSizeF; }
    //Converts world coordinates to the world chunk coordinate of the nearest voxel.
    static Vector3i ToWorldVoxelIndex(Vector3f worldSpace) { return (worldSpace / VoxelSizeF).Floored(); }

    //Converts the given shape's bounding box to World-Chunk-Space indices.
    static MinMaxI GetShapeBoundsI(const Shape & shpe);
    //Converts the given shape's bounding box to World-Chunk-Space indices.
    static MinMaxF GetShapeBoundsF(const Shape & shpe);


    //Converters between World Space and Local Chunk Space.

    //Converts local chunk coordinates to world Coordinates.
    Vector3f LocalToWorldSpace(Vector3i chunkCoord) const { return LocalToWorldSpace(Vector3f((float)chunkCoord.x, (float)chunkCoord.y, (float)chunkCoord.z)); }
    //Converts local chunk coordinates to world Coordinates.
    Vector3f LocalToWorldSpace(Vector3f chunkCoord) const { return (chunkCoord * VoxelSizeF) + Vector3f((float)MinCorner.x, (float)MinCorner.y, (float)MinCorner.z); }

    //Converts world coordinates to local Chunk coordinates.
    Vector3f ToLocalChunkSpace(Vector3f worldSpace) const { return ToWorldChunkSpace(worldSpace - Vector3f((float)MinCorner.x, (float)MinCorner.y, (float)MinCorner.z)); }
    //Converts world coordinates to the coordinate of the nearest local voxel.
    Vector3i ToLocalVoxelIndex(Vector3f worldSpace) const { return ToWorldVoxelIndex(worldSpace - Vector3f((float)MinCorner.x, (float)MinCorner.y, (float)MinCorner.z)); }


    //Clamps the given Local-Chunk-Space coordinate to be inside this Chunk.
    Vector3i Clamp(Vector3i inV) const { return inV.Clamp(0, ChunkSize); }
    //Clamps the given Local-Chunk-Space coordinate to be inside this Chunk.
    Vector3f Clamp(Vector3f inV) const { return inV.Clamp(0.0f, ChunkSizeF); }

    //Converts the given shape's bounding box to Local-Chunk-Space indices.
    MinMaxI GetLocalShapeBoundsI(const Shape & shpe) const;
    //Converts the given shape's bounding box to Local-Chunk-Space indices.
    MinMaxF GetLocalShapeBoundsF(const Shape & shpe) const;


    //Basic voxel getters/setters.

    //Gets the number of solid voxels in this chunk.
    unsigned int GetNumbSolidVoxels(void) const { return nSolidVoxels; }
    //Gets whether or not this chunk is empty.
    bool IsEmpty(void) const { return nSolidVoxels == 0; }
    //Gets whether or not this chunk is full.
    bool IsFull(void) const { return nSolidVoxels == (ChunkSize * ChunkSize * ChunkSize); }

    //Gets the voxels.
    const Array3D<bool> & GetVoxels(void) const { return voxels; }

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


    //More advanced Voxel getters/setters.

    //Gets whether ANY voxels with the majority of their volume inside the given Shape are solid.
    bool GetAnyVoxels(const Shape & shpe) const;
    //Gets whether ALL voxels with the majority of their volume inside the given Shape are solid.
    bool GetAllVoxels(const Shape & shpe) const;
    //Toggles all voxels with the majority of their volume inside the given Shape.
    void ToggleVoxels(const Shape & shpe);
    //Sets all voxels with the majority of their volume inside the given Shape.
    void SetVoxels(const Shape & shpe, bool value);


    //Iterators through a box of voxels.

    template<typename Func>
    //"Func" must have the signature "bool Func(Vector3i localIndex)".
    //"Func" returns whether to exit "DoToEveryVoxel" after calling it.
    //Calls "todo" on every valid local voxel index between "start" and "end", inclusive.
    //Returns whether or not "todo" ever returned "true".
    bool DoToEveryVoxelPredicate(Func todo, Vector3i start = Vector3i(0, 0, 0), Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1))
    {
        Vector3i sign(BasicMath::Sign(end.x - start.x), BasicMath::Sign(end.y - start.y), BasicMath::Sign(end.z - start.z));
        if (sign.x == 0) sign.x = 1;
        if (sign.y == 0) sign.y = 1;
        if (sign.z == 0) sign.z = 1;

        if ((sign.x > 0 && (end.x < 0 || start.x > ChunkSize - 1)) ||
            (sign.x < 0 && (end.x > ChunkSize - 1 || start.x < 0)) ||
            (sign.y > 0 && (end.y < 0 || start.y > ChunkSize - 1)) ||
            (sign.y < 0 && (end.y > ChunkSize - 1 || start.y < 0)) ||
            (sign.z > 0 && (end.z < 0 || start.z > ChunkSize - 1)) ||
            (sign.z < 0 && (end.z > ChunkSize - 1 || start.z < 0)))
        {
            return false;
        }

        int xStart = BasicMath::Clamp<int>(start.x, 0, ChunkSize - 1),
            yStart = BasicMath::Clamp<int>(start.y, 0, ChunkSize - 1),
            zStart = BasicMath::Clamp<int>(start.z, 0, ChunkSize - 1);
        int xEnd = BasicMath::Clamp<int>(end.x, 0, ChunkSize - 1),
            yEnd = BasicMath::Clamp<int>(end.y, 0, ChunkSize - 1),
            zEnd = BasicMath::Clamp<int>(end.z, 0, ChunkSize - 1);

        Vector3i loc;
        for (loc.z = zStart; (sign.z > 0 && loc.z <= zEnd) || (sign.z < 0 && loc.z >= zEnd); loc.z += sign.z)
            for (loc.y = yStart; (sign.y > 0 && loc.y <= yEnd) || (sign.y < 0 && loc.y >= yEnd); loc.y += sign.y)
                for (loc.x = xStart; (sign.x > 0 && loc.x <= xEnd) || (sign.x < 0 && loc.x >= xEnd); loc.x += sign.x)
                    if (todo(loc))
                        return true;
        return false;
    }
    template<typename Func>
    //"Func" must have the signature "void Func(Vector3i localIndex)".
    //Calls "todo" on every valid local voxel index between "start" and "end", inclusive.
    void DoToEveryVoxel(Func todo, Vector3i start = Vector3i(0, 0, 0), Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1))
    {
        Vector3i sign(BasicMath::Sign(end.x - start.x), BasicMath::Sign(end.y - start.y), BasicMath::Sign(end.z - start.z));
        if (sign.x == 0) sign.x = 1;
        if (sign.y == 0) sign.y = 1;
        if (sign.z == 0) sign.z = 1;

        if ((sign.x > 0 && (end.x < 0 || start.x > (int)(ChunkSize - 1))) ||
            (sign.x < 0 && (end.x > ChunkSize - 1 || start.x < 0)) ||
            (sign.y > 0 && (end.y < 0 || start.y > (int)(ChunkSize - 1))) ||
            (sign.y < 0 && (end.y > ChunkSize - 1 || start.y < 0)) ||
            (sign.z > 0 && (end.z < 0 || start.z > (int)(ChunkSize - 1))) ||
            (sign.z < 0 && (end.z > ChunkSize - 1 || start.z < 0)))
        {
            return;
        }

        int xStart = BasicMath::Clamp<int>(start.x, 0, ChunkSize - 1),
            yStart = BasicMath::Clamp<int>(start.y, 0, ChunkSize - 1),
            zStart = BasicMath::Clamp<int>(start.z, 0, ChunkSize - 1);
        int xEnd = BasicMath::Clamp<int>(end.x, 0, ChunkSize - 1),
            yEnd = BasicMath::Clamp<int>(end.y, 0, ChunkSize - 1),
            zEnd = BasicMath::Clamp<int>(end.z, 0, ChunkSize - 1);

        Vector3i loc;
        for (loc.z = zStart; (sign.z > 0 && loc.z <= zEnd) || (sign.z < 0 && loc.z >= zEnd); loc.z += sign.z)
            for (loc.y = yStart; (sign.y > 0 && loc.y <= yEnd) || (sign.y < 0 && loc.y >= yEnd); loc.y += sign.y)
                for (loc.x = xStart; (sign.x > 0 && loc.x <= xEnd) || (sign.x < 0 && loc.x >= xEnd); loc.x += sign.x)
                    todo(loc);
    }
    template<typename Func>
    //"Func" must have the signature "bool Func(Vector3i localIndex)".
    //"Func" returns whether to exit "DoToEveryVoxel" after calling it.
    //Calls "todo" on every valid local voxel index between "start" and "end", inclusive.
    //Returns whether or not "todo" ever returned "true".
    bool DoToEveryVoxelPredicate(Func todo, Vector3i start = Vector3i(0, 0, 0), Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1)) const
    {
        Vector3i sign(BasicMath::Sign(end.x - start.x), BasicMath::Sign(end.y - start.y), BasicMath::Sign(end.z - start.z));
        if (sign.x == 0) sign.x = 1;
        if (sign.y == 0) sign.y = 1;
        if (sign.z == 0) sign.z = 1;

        if ((sign.x > 0 && (end.x < 0 || start.x > ChunkSize - 1)) ||
            (sign.x < 0 && (end.x > ChunkSize - 1 || start.x < 0)) ||
            (sign.y > 0 && (end.y < 0 || start.y > ChunkSize - 1)) ||
            (sign.y < 0 && (end.y > ChunkSize - 1 || start.y < 0)) ||
            (sign.z > 0 && (end.z < 0 || start.z > ChunkSize - 1)) ||
            (sign.z < 0 && (end.z > ChunkSize - 1 || start.z < 0)))
        {
            return false;
        }

        int xStart = BasicMath::Clamp<int>(start.x, 0, ChunkSize - 1),
            yStart = BasicMath::Clamp<int>(start.y, 0, ChunkSize - 1),
            zStart = BasicMath::Clamp<int>(start.z, 0, ChunkSize - 1);
        int xEnd = BasicMath::Clamp<int>(end.x, 0, ChunkSize - 1),
            yEnd = BasicMath::Clamp<int>(end.y, 0, ChunkSize - 1),
            zEnd = BasicMath::Clamp<int>(end.z, 0, ChunkSize - 1);

        Vector3i loc;
        for (loc.z = zStart; (sign.z > 0 && loc.z <= zEnd) || (sign.z < 0 && loc.z >= zEnd); loc.z += sign.z)
            for (loc.y = yStart; (sign.y > 0 && loc.y <= yEnd) || (sign.y < 0 && loc.y >= yEnd); loc.y += sign.y)
                for (loc.x = xStart; (sign.x > 0 && loc.x <= xEnd) || (sign.x < 0 && loc.x >= xEnd); loc.x += sign.x)
                    if (todo(loc))
                        return true;
        return false;
    }
    template<typename Func>
    //"Func" must have the signature "void Func(Vector3i localIndex)".
    //Calls "todo" on every valid local voxel index between "start" and "end", inclusive.
    void DoToEveryVoxel(Func todo, Vector3i start = Vector3i(0, 0, 0), Vector3i end = Vector3i(ChunkSize - 1, ChunkSize - 1, ChunkSize - 1)) const
    {
        Vector3i sign(BasicMath::Sign(end.x - start.x), BasicMath::Sign(end.y - start.y), BasicMath::Sign(end.z - start.z));
        if (sign.x == 0) sign.x = 1;
        if (sign.y == 0) sign.y = 1;
        if (sign.z == 0) sign.z = 1;

        if ((sign.x > 0 && (end.x < 0 || start.x > ChunkSize - 1)) ||
            (sign.x < 0 && (end.x > ChunkSize - 1 || start.x < 0)) ||
            (sign.y > 0 && (end.y < 0 || start.y > ChunkSize - 1)) ||
            (sign.y < 0 && (end.y > ChunkSize - 1 || start.y < 0)) ||
            (sign.z > 0 && (end.z < 0 || start.z > ChunkSize - 1)) ||
            (sign.z < 0 && (end.z > ChunkSize - 1 || start.z < 0)))
        {
            return;
        }

        int xStart = BasicMath::Clamp<int>(start.x, 0, ChunkSize - 1),
            yStart = BasicMath::Clamp<int>(start.y, 0, ChunkSize - 1),
            zStart = BasicMath::Clamp<int>(start.z, 0, ChunkSize - 1);
        int xEnd = BasicMath::Clamp<int>(end.x, 0, ChunkSize - 1),
            yEnd = BasicMath::Clamp<int>(end.y, 0, ChunkSize - 1),
            zEnd = BasicMath::Clamp<int>(end.z, 0, ChunkSize - 1);

        Vector3i loc;
        for (loc.z = zStart; (sign.z > 0 && loc.z <= zEnd) || (sign.z < 0 && loc.z >= zEnd); loc.z += sign.z)
            for (loc.y = yStart; (sign.y > 0 && loc.y <= yEnd) || (sign.y < 0 && loc.y >= yEnd); loc.y += sign.y)
                for (loc.x = xStart; (sign.x > 0 && loc.x <= xEnd) || (sign.x < 0 && loc.x >= xEnd); loc.x += sign.x)
                    todo(loc);
    }


    struct VoxelRayHit
    {
    public:
        Vector3i VoxelIndex;
        Shape::RayTraceResult CastResult;
        //Has one of the following values: { +/-1, 0, 0 }, { 0, +/-1, 0}, { 0, 0, +/-1}.
        //Indicates which face of the cube was hit.
        //A value of { 0, 0, 0 } indicates that the field was never properly set.
        Vector3i Face;
        VoxelRayHit(void) : VoxelIndex(-1, -1, -1) { }
    };
    //Returns the ray cast and the index of the first voxel that is hit by the given ray
    //    (in Local Chunk space), or Vector3i(-1, -1, -1) if nothing was hit.
    VoxelRayHit CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist = ChunkSizeF + ChunkSizeF) const;

    //TODO: Put thsi into "VoxelMesh" class.
    //Builds the world-space triangles/indices for this chunk, given all surrounding chunks.
    //Any of the surrounding chunks passed in may have values of 0 if they don't exist.
    void BuildTriangles(std::vector<VoxelVertex> & vertices,
                        const VoxelChunk * beforeMinX, const VoxelChunk * afterMaxX,
                        const VoxelChunk * beforeMinY, const VoxelChunk * afterMaxY,
                        const VoxelChunk * beforeMinZ, const VoxelChunk * afterMaxZ) const;


    //Gets the world-space bounds around this chunk.
    Box3D GetBounds(void) const
    {
        return Box3D((float)MinCorner.x, (float)MinCorner.y, (float)MinCorner.z,
                     Vector3f(ChunkSizeF * VoxelSizeF, ChunkSizeF * VoxelSizeF, ChunkSizeF * VoxelSizeF));
    }
    //Gets the world-space bounds around the given voxel (local chunk space).
    Box3D GetBounds(Vector3i chunkIndex) const
    {
        Vector3f dims(VoxelSizeF, VoxelSizeF, VoxelSizeF),
                 minCorner = LocalToWorldSpace(chunkIndex);
        return Box3D(minCorner.x, minCorner.y, minCorner.z, dims);
    }


private:

    Array3D<bool> voxels;
    unsigned int nSolidVoxels;
};