#include "VoxelChunk.h"
#include "../Math/Shapes/ThreeDShapes.h"

typedef VoxelChunk VC;


const int VC::ChunkSize = 30;
const float VC::ChunkSizeF = 30.0f;

const int VC::VoxelSize = 1;
const float VC::VoxelSizeF = 1.0f;


VC::MinMaxI VC::GetShapeBoundsI(const Shape & shpe)
{
    MinMaxI ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = ToWorldVoxelIndex(bounds.GetMinCorner());
    ret.Max = ToWorldVoxelIndex(bounds.GetMaxCorner());

    return ret;
}
VC::MinMaxI VC::GetLocalShapeBoundsI(const Shape & shpe) const
{
    MinMaxI ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = ToLocalVoxelIndex(bounds.GetMinCorner());
    ret.Max = ToLocalVoxelIndex(bounds.GetMaxCorner());

    return ret;
}

VC::MinMaxF VC::GetShapeBoundsF(const Shape & shpe)
{
    MinMaxF ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = ToWorldChunkSpace(bounds.GetMinCorner());
    ret.Max = ToWorldChunkSpace(bounds.GetMaxCorner());

    return ret;
}
VC::MinMaxF VC::GetLocalShapeBoundsF(const Shape & shpe) const
{
    MinMaxF ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = ToLocalChunkSpace(bounds.GetMinCorner());
    ret.Max = ToLocalChunkSpace(bounds.GetMaxCorner());

    return ret;
}


bool VC::GetAnyVoxels(const Shape & shpe) const
{
    MinMaxI bounds = GetLocalShapeBoundsI(shpe);

    const VC * thisVC = this;
    return DoToEveryVoxelPredicate([&shpe, thisVC](Vector3i lc) -> bool
    {
        return (shpe.IsPointInside(thisVC->LocalToWorldSpace(lc)) &&
                thisVC->GetVoxelLocal(lc));
    }, bounds.Min, bounds.Max);
}
bool VC::GetAllVoxels(const Shape & shpe) const
{
    MinMaxI bounds = GetLocalShapeBoundsI(shpe);

    const VC * thisVC = this;
    return !DoToEveryVoxelPredicate([&shpe, thisVC](Vector3i lc) -> bool
    {
        return (shpe.IsPointInside(thisVC->LocalToWorldSpace(lc)) &&
                !thisVC->GetVoxelLocal(lc));
    }, bounds.Min, bounds.Max);
}

void VC::ToggleVoxels(const Shape & shpe)
{
    MinMaxI bounds = GetLocalShapeBoundsI(shpe);

    VC * thisVC = this;
    DoToEveryVoxel([&shpe, thisVC](Vector3i lc)
    {
        if (shpe.IsPointInside(thisVC->LocalToWorldSpace(lc)))
            thisVC->ToggleVoxelLocal(lc);
    }, bounds.Min, bounds.Max);
}
void VC::SetVoxels(const Shape & shpe, bool value)
{
    MinMaxI bounds = GetLocalShapeBoundsI(shpe);

    VC * thisVC = this;
    DoToEveryVoxel([&shpe, thisVC, value](Vector3i lc)
    {
        if (shpe.IsPointInside(thisVC->LocalToWorldSpace(lc)))
            thisVC->SetVoxelLocal(lc, value);
    }, bounds.Min, bounds.Max);
}

Vector3i VC::CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist) const
{
    rayStart = ToLocalChunkSpace(rayStart);
    maxDist /= VoxelSizeF;

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


    //Find the axis that moves the fastest, and figure out what
    //   't' increment is needed to move the ray one unit along that axis.

    float tIncrement = 1.0f / rayDir[GeometricMath::GetLongestAxis(rayDir)];


    //March the ray in increments of that amount and check the bounding area
    //    from the last ray spot to the current ray spot.

    const VoxelChunk * thisVC = this;
    Vector3i lastRayPos = ToLocalVoxelIndex(rayStart);
    Vector3i currentRayPos;

    float distTraveled = 0.0f;
    Vector3f moveIncrement = rayDir * tIncrement;

    //Shortcuts to quickly check the ray's position after every iteration.
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

    while (distTraveled - 0.5f <= maxDist &&
           (!isXTooFar(rayStart.x) || !isYTooFar(rayStart.y) || !isZTooFar(rayStart.z)))
    {
        currentRayPos = ToLocalVoxelIndex(rayStart);

        //Go through every voxel between the previous ray position and the current.
        //If any of them have a solid voxel, return that voxel's index.
        Vector3i checkPos;
        if (DoToEveryVoxelPredicate([&checkPos, thisVC](Vector3i loc)
        {
            if (thisVC->GetVoxelLocal(loc))
            {
                checkPos = loc;
                return true;
            }
            return false;
        }, lastRayPos, currentRayPos))
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
    for (startLoc.z = 0; startLoc.z < ChunkSize; ++startLoc.z)
    {
        voxelStart.z = (VoxelSizeF * startLoc.z) + worldOffset.z;
        voxelEnd.z = voxelStart.z + VoxelSizeF;

        for (startLoc.y = 0; startLoc.y < ChunkSize; ++startLoc.y)
        {
            voxelStart.y = (VoxelSizeF * startLoc.y) + worldOffset.y;
            voxelEnd.y = voxelStart.y + VoxelSizeF;

            for (startLoc.x = 0; startLoc.x < ChunkSize; ++startLoc.x)
            {
                voxelStart.x = (VoxelSizeF * startLoc.x) + worldOffset.x;
                voxelEnd.x = voxelStart.x + VoxelSizeF;

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