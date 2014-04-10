#include "VoxelChunk.h"
#include "../Math/Shapes/ThreeDShapes.h"

typedef VoxelChunk VC;


const int VC::ChunkSize = 30;
const float VC::ChunkSizeF = 30.0f;

const int VC::VoxelSize = 1;
const float VC::VoxelSizeF = 1.0f;


struct MinMax { public: Vector3i Min, Max; };
//Converts the given shape's bounding box to min and max local voxel indices.
//Translates the bounds to be relative to the given chunk MinCorner.
//The bounds may be outside the chunk's bounds.
MinMax GetShapeBounds(const Shape & shpe, const VoxelChunk * chnk)
{
    MinMax ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = chnk->ToLocalVoxelIndex(bounds.GetMinCorner());
    ret.Max = chnk->ToLocalVoxelIndex(bounds.GetMaxCorner());

    return ret;
}


bool VC::GetAnyVoxels(const Shape & shpe) const
{
    MinMax bounds = GetShapeBounds(shpe, this);

    if (bounds.Min.x >= ChunkSize || bounds.Min.y >= ChunkSize || bounds.Min.z >= ChunkSize ||
        bounds.Max.x < 0 || bounds.Max.y < 0 || bounds.Max.z < 0)
        return false;

    Vector3i loc;
    Vector3f locF;
    for (loc.x = BasicMath::Max(0, bounds.Min.x); loc.x <= bounds.Max.x && loc.x < ChunkSize; ++loc.x)
    {
        locF.x = loc.x;
        for (loc.y = BasicMath::Max(0, bounds.Min.y); loc.y <= bounds.Max.y && loc.y < ChunkSize; ++loc.y)
        {
            locF.y = loc.y;
            for (loc.z = BasicMath::Max(0, bounds.Min.z); loc.z <= bounds.Max.z && loc.z < ChunkSize; ++loc.z)
            {
                locF.z = loc.z;

                if (shpe.IsPointInside(LocalToWorldSpace(locF)) && GetVoxelLocal(loc))
                    return true;
            }
        }
    }

    return false;
}
bool VC::GetAllVoxels(const Shape & shpe) const
{
    MinMax bounds = GetShapeBounds(shpe, this);

    if (bounds.Min.x >= ChunkSize || bounds.Min.y >= ChunkSize || bounds.Min.z >= ChunkSize ||
        bounds.Max.x < 0 || bounds.Max.y < 0 || bounds.Max.z < 0)
        return true;

    Vector3i loc;
    Vector3f locF;
    for (loc.x = BasicMath::Max(0, bounds.Min.x); loc.x <= bounds.Max.x && loc.x < ChunkSize; ++loc.x)
    {
        locF.x = loc.x;
        for (loc.y = BasicMath::Max(0, bounds.Min.y); loc.y <= bounds.Max.y && loc.y < ChunkSize; ++loc.y)
        {
            locF.y = loc.y;
            for (loc.z = BasicMath::Max(0, bounds.Min.z); loc.z <= bounds.Max.z && loc.z < ChunkSize; ++loc.z)
            {
                locF.z = loc.z;

                if (shpe.IsPointInside(LocalToWorldSpace(locF)) && !GetVoxelLocal(loc))
                    return false;
            }
        }
    }

    return true;
}

void VC::ToggleVoxels(const Shape & shpe)
{
    MinMax bounds = GetShapeBounds(shpe, this);

    if (bounds.Min.x >= ChunkSize || bounds.Min.y >= ChunkSize || bounds.Min.z >= ChunkSize ||
        bounds.Max.x < 0 || bounds.Max.y < 0 || bounds.Max.z < 0)
        return;

    Vector3i loc;
    Vector3f locF;
    for (loc.x = BasicMath::Max(0, bounds.Min.x); loc.x <= bounds.Max.x && loc.x < ChunkSize; ++loc.x)
    {
        locF.x = loc.x;
        for (loc.y = BasicMath::Max(0, bounds.Min.y); loc.y <= bounds.Max.y && loc.y < ChunkSize; ++loc.y)
        {
            locF.y = loc.y;
            for (loc.z = BasicMath::Max(0, bounds.Min.z); loc.z <= bounds.Max.z && loc.z < ChunkSize; ++loc.z)
            {
                locF.z = loc.z;

                if (shpe.IsPointInside(LocalToWorldSpace(locF)))
                    ToggleVoxelLocal(loc);
            }
        }
    }
}
void VC::SetVoxels(const Shape & shpe, bool value)
{
    MinMax bounds = GetShapeBounds(shpe, this);

    if (bounds.Min.x >= ChunkSize || bounds.Min.y >= ChunkSize || bounds.Min.z >= ChunkSize ||
        bounds.Max.x < 0 || bounds.Max.y < 0 || bounds.Max.z < 0)
        return;

    Vector3i loc;
    Vector3f locF;
    for (loc.x = BasicMath::Max(0, bounds.Min.x); loc.x <= bounds.Max.x && loc.x < ChunkSize; ++loc.x)
    {
        locF.x = loc.x;
        for (loc.y = BasicMath::Max(0, bounds.Min.y); loc.y <= bounds.Max.y && loc.y < ChunkSize; ++loc.y)
        {
            locF.y = loc.y;
            for (loc.z = BasicMath::Max(0, bounds.Min.z); loc.z <= bounds.Max.z && loc.z < ChunkSize; ++loc.z)
            {
                locF.z = loc.z;

                if (shpe.IsPointInside(LocalToWorldSpace(locF)))
                    SetVoxelLocal(loc, value);
            }
        }
    }
}

Vector3i VC::CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist) const
{
    rayStart = ToLocalChunkSpace(rayStart);

    //If the ray isn't even pointing towards the chunk, don't bother raycasting.
    if (IsEmpty() ||
        (rayStart.x < 0.0f && rayDir.x <= 0.0f) ||
        (rayStart.y < 0.0f && rayDir.y <= 0.0f) ||
        (rayStart.z < 0.0f && rayDir.z <= 0.0f) ||
        (rayStart.x >= ChunkSizeF && rayDir.x >= 0.0f) ||
        (rayStart.y >= ChunkSizeF && rayDir.y >= 0.0f) ||
        (rayStart.z >= ChunkSizeF && rayDir.z >= 0.0f))
    {
        return Vector3i(-1, -1, -1);
    }


    Box3D bounds = GetBounds();


    //Find the axis that moves the most, and figure out what
    //   't' increment is needed to move the ray one unit along that axis.

    int axis, axis2, axis3;
    Vector3f abs(BasicMath::Abs(rayDir.x), BasicMath::Abs(rayDir.y), BasicMath::Abs(rayDir.z));

    if (abs.x > abs.y && abs.x > abs.z)
    {
        axis = 0;
        axis2 = 1;
        axis3 = 2;
    }
    else if (abs.y > abs.x && abs.y > abs.z)
    {
        axis = 1;
        axis2 = 2;
        axis3 = 0;
    }
    else
    {
        axis = 2;
        axis2 = 0;
        axis3 = 1;
    }

    float tIncrement = 1.0f / rayDir[axis];


    //March the ray in increments of that amount and check the bounding area
    //    from the last ray spot to the current ray spot.

    Vector3i lastRayPos = Clamp(ToLocalVoxelIndex(rayStart));
    float distTraveled = 0.0f;
    Vector3f moveIncrement = rayDir * tIncrement;

    typedef bool(*FloatChecker)(float vec);
    FloatChecker isXTooFar, isYTooFar, isZTooFar;
    if (rayDir.x > 0.0f)
        isXTooFar = ([](float rayPos) -> bool { return rayPos >= ChunkSizeF; });
    else isXTooFar = ([](float rayPos) -> bool { return rayPos < 0.0f; });
    if (rayDir.y > 0.0f)
        isYTooFar = ([](float rayPos) -> bool { return rayPos >= ChunkSizeF; });
    else isYTooFar = ([](float rayPos) -> bool { return rayPos < 0.0f; });
    if (rayDir.z > 0.0f)
        isZTooFar = ([](float rayPos) -> bool { return rayPos >= ChunkSizeF; });
    else isZTooFar = ([](float rayPos) -> bool { return rayPos < 0.0f; });

    while (distTraveled <= maxDist && !isXTooFar(rayStart.x) && !isYTooFar(rayStart.y) && !isZTooFar(rayStart.z))
    {
        Vector3i currentRayPos = Clamp(ToLocalVoxelIndex(rayStart));

        Vector3i checkPos;
        for (checkPos.x = lastRayPos.x; checkPos.x <= currentRayPos.x; ++checkPos.x)
            for (checkPos.y = lastRayPos.y; checkPos.y <= currentRayPos.y; ++checkPos.y)
                for (checkPos.z = lastRayPos.z; checkPos.z <= currentRayPos.z; ++checkPos.z)
                    if (GetVoxelLocal(checkPos))
                        return checkPos;

        lastRayPos = currentRayPos;
        rayStart += moveIncrement;
        distTraveled += tIncrement;
    }

    return Vector3i(-1, -1, -1);
}

void VC::BuildTriangles(std::vector<Vector3f> & vertices, std::vector<unsigned int> & indices,
                        const VoxelChunk * beforeMinX, const VoxelChunk * afterMaxX,
                        const VoxelChunk * beforeMinY, const VoxelChunk * afterMaxY,
                        const VoxelChunk * beforeMinZ, const VoxelChunk * afterMaxZ) const
{
    if (IsEmpty()) return;
    if (IsFull())
    {
        //TODO: Implement shortcut.

        return;
    }


    //TODO: Use different methods depending on the ratio of solid to empty voxels in this chunk (either build by empty voxels or by solid voxels)?


    //Get some functions to get a voxel from a surrounding chunk.

    typedef bool(*GetOtherChunkVal)(Vector3i location, const VoxelChunk * otherCnk);
    GetOtherChunkVal noVal = [](Vector3i loc, const VoxelChunk * other) { return false; };

    GetOtherChunkVal gMinX, gMinY, gMinZ, gMaxX, gMaxY, gMaxZ;

    if (beforeMinX == 0) gMinX = noVal;
    else gMinX = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x - ChunkSize, loc.y, loc.z));
    };

    if (beforeMinY == 0) gMinY = noVal;
    else gMinY = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x, loc.y - ChunkSize, loc.z));
    };

    if (beforeMinZ == 0) gMinZ = noVal;
    else gMinZ = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x, loc.y, loc.z - ChunkSize));
    };

    if (afterMaxX == 0) gMaxX = noVal;
    else gMaxX = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x + ChunkSize, loc.y, loc.z));
    };

    if (afterMaxY == 0) gMaxY = noVal;
    else gMaxY = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x, loc.y + ChunkSize, loc.z));
    };

    if (afterMaxZ == 0) gMaxZ = noVal;
    else gMaxZ = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x, loc.y, loc.z + ChunkSize));
    };


    //For every empty voxel, create the triangles for any solid voxels touching it.

    Vector3i startLoc;
    Vector3f worldOffset = ToWorldSpace(MinCorner);

    Vector3f voxelStart, voxelEnd;
    int startingIndex = 0;
    for (startLoc.x = 0; startLoc.x < ChunkSize; ++startLoc.x)
    {
        voxelStart.x = (VoxelSizeF * startLoc.x) + worldOffset.x;
        voxelEnd.x = voxelStart.x + VoxelSizeF;

        for (startLoc.y = 0; startLoc.y < ChunkSize; ++startLoc.y)
        {
            voxelStart.y = (VoxelSizeF * startLoc.y) + worldOffset.y;
            voxelEnd.y = voxelStart.y + VoxelSizeF;

            for (startLoc.z = 0; startLoc.z < ChunkSize; ++startLoc.z)
            {
                voxelStart.z = (VoxelSizeF * startLoc.z) + worldOffset.z;
                voxelEnd.z = voxelStart.z + VoxelSizeF;

                if (!voxels[startLoc])
                {
                    Vector3i xMin = Vector3i(startLoc.x - 1, startLoc.y, startLoc.z),
                             xMax = Vector3i(startLoc.x + 1, startLoc.y, startLoc.z),
                             yMin = Vector3i(startLoc.x, startLoc.y - 1, startLoc.z),
                             yMax = Vector3i(startLoc.x, startLoc.y + 1, startLoc.z),
                             zMin = Vector3i(startLoc.x, startLoc.y, startLoc.z - 1),
                             zMax = Vector3i(startLoc.x, startLoc.y, startLoc.z + 1);

                    if ((xMin.x >= 0 && voxels[xMin]) ||
                        (xMin.x < 0 && gMinX(xMin, beforeMinX)))
                    {
                        vertices.insert(vertices.end(), voxelStart);
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelStart.y, voxelEnd.z));
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelEnd.y,   voxelStart.z));
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelEnd.y,   voxelEnd.z));

                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 1);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex + 2);

                        startingIndex += 4;
                    }
                    if ((yMin.y >= 0 && voxels[yMin]) ||
                        (yMin.y < 0 && gMinY(yMin, beforeMinY)))
                    {
                        vertices.insert(vertices.end(), voxelStart);
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelStart.y, voxelEnd.z));
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x,   voxelStart.y, voxelStart.z));
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x,   voxelStart.y, voxelEnd.z));

                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 1);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex + 2);

                        startingIndex += 4;
                    }
                    if ((zMin.z >= 0 && voxels[zMin]) ||
                        (zMin.z < 0 && gMinZ(zMin, beforeMinZ)))
                    {
                        vertices.insert(vertices.end(), voxelStart);
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelEnd.y,   voxelStart.z));
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x,   voxelStart.y, voxelStart.z));
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x,   voxelEnd.y,   voxelStart.z));

                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 1);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex + 2);

                        startingIndex += 4;
                    }
                    if ((xMax.x < ChunkSize && voxels[xMax]) ||
                        (xMax.x >= ChunkSize && gMaxX(xMax, afterMaxX)))
                    {
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x, voxelStart.y, voxelStart.z));
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x, voxelStart.y, voxelEnd.z));
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x, voxelEnd.y,   voxelStart.z));
                        vertices.insert(vertices.end(), voxelEnd);

                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 1);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex + 2);

                        startingIndex += 4;
                    }
                    if ((yMax.y < ChunkSize && voxels[yMax]) ||
                        (yMax.y >= ChunkSize && gMaxY(yMax, afterMaxY)))
                    {
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelEnd.y, voxelStart.z));
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelEnd.y, voxelEnd.z));
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x,   voxelEnd.y, voxelStart.z));
                        vertices.insert(vertices.end(), voxelEnd);

                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 1);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex + 2);

                        startingIndex += 4;
                    }
                    if ((zMax.z < ChunkSize && voxels[zMax]) ||
                        (zMax.z >= ChunkSize && gMaxZ(zMax, afterMaxZ)))
                    {
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelStart.y, voxelEnd.z));
                        vertices.insert(vertices.end(), Vector3f(voxelStart.x, voxelEnd.y,   voxelEnd.z));
                        vertices.insert(vertices.end(), Vector3f(voxelEnd.x,   voxelStart.y, voxelEnd.z));
                        vertices.insert(vertices.end(), voxelEnd);

                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 1);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex);
                        indices.insert(indices.end(), startingIndex + 3);
                        indices.insert(indices.end(), startingIndex + 2);

                        startingIndex += 4;
                    }
                }
            }
        }
    }
    //TODO: Go through every chunk side and build all quads along that side.
}