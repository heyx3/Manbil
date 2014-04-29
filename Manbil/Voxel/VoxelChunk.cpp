#include "VoxelChunk.h"

#include <iostream>


typedef VoxelChunk VC;


const int VC::ChunkSize = 30;
const float VC::ChunkSizeF = 30.0f;

const int VC::VoxelSize = 3;
const float VC::VoxelSizeF = 3.0f;


VC::MinMaxI VC::GetShapeBoundsI(const Shape & shpe)
{
    MinMaxI ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = ToWorldVoxelIndex(bounds.GetMinCorner());
    ret.Max = ToWorldVoxelIndex(bounds.GetMaxCorner());

    ret.Max += Vector3i(1, 1, 1);

    return ret;
}
VC::MinMaxI VC::GetLocalShapeBoundsI(const Shape & shpe) const
{
    MinMaxI ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = ToLocalVoxelIndex(bounds.GetMinCorner());
    ret.Max = ToLocalVoxelIndex(bounds.GetMaxCorner());

    ret.Max += Vector3i(1, 1, 1);

    return ret;
}

VC::MinMaxF VC::GetShapeBoundsF(const Shape & shpe)
{
    MinMaxF ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = ToWorldChunkSpace(bounds.GetMinCorner());
    ret.Max = ToWorldChunkSpace(bounds.GetMaxCorner());

    ret.Max += Vector3f(1.0f, 1.0f, 1.0f);

    return ret;
}
VC::MinMaxF VC::GetLocalShapeBoundsF(const Shape & shpe) const
{
    MinMaxF ret;
    Box3D bounds = shpe.GetBoundingBox();

    ret.Min = ToLocalChunkSpace(bounds.GetMinCorner());
    ret.Max = ToLocalChunkSpace(bounds.GetMaxCorner());

    ret.Max += Vector3f(1.0f, 1.0f, 1.0f);

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

VC::VoxelRayHit VC::CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist) const
{
    VoxelRayHit vrh;

    Vector3f worldRayStart = rayStart;
    rayStart = ToLocalChunkSpace(rayStart);
    maxDist /= VoxelSizeF;

    Box3D bounds = GetBounds();


    //If the ray isn't even pointing towards the chunk, don't bother raycasting.
    if (IsEmpty() ||
        (rayStart.x < 0.0f && rayDir.x <= 0.0f) ||
        (rayStart.y < 0.0f && rayDir.y <= 0.0f) ||
        (rayStart.z < 0.0f && rayDir.z <= 0.0f) ||
        (rayStart.x >= ChunkSizeF && rayDir.x >= 0.0f) ||
        (rayStart.y >= ChunkSizeF && rayDir.y >= 0.0f) ||
        (rayStart.z >= ChunkSizeF && rayDir.z >= 0.0f))
    {
        return vrh;
    }


    //Find the axis that moves the fastest, and figure out what
    //   't' increment is needed to move the ray one unit along that axis.

    int axis = GeometricMath::GetLongestAxis(rayDir);
    float tIncrement = GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, axis,
                                                            rayStart[axis] + BasicMath::Sign(rayDir[axis])).t;


    //March the ray in increments of that amount and check the bounding area
    //    from the last ray spot to the current ray spot.

    const VoxelChunk * thisVC = this;
    Vector3i lastRayPos = rayStart.Floored();
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
        currentRayPos = rayStart.Floored();

        //Go through every voxel between the previous ray position and the current.
        //If any of them have a solid voxel, return that voxel's index.
        if (DoToEveryVoxelPredicate([&vrh, thisVC, worldRayStart, rayDir](Vector3i loc)
        {
            if (thisVC->GetVoxelLocal(loc))
            {
                Shape::RayTraceResult res = Cube(thisVC->GetBounds(loc)).RayHitCheck(worldRayStart, rayDir);
                if (res.DidHitTarget)
                {
                    vrh.VoxelIndex = loc;
                    vrh.CastResult = res;
                    return true;
                }
            }
            return false;
        }, lastRayPos, currentRayPos))
            break;

        lastRayPos = currentRayPos;
        rayStart += moveIncrement;
        worldRayStart += moveIncrement;
        distTraveled += tIncrement;
    }

    return vrh;
}



//Creates a quad on the given x face of a cube bound by the given min/max points.
void CreateXAxisQuad(std::vector<Vector3f> & poses, std::vector<Vector3f> & normals, std::vector<Vector2f> & texCoords, std::vector<unsigned int> & indices,
                     unsigned int & startingIndex, Vector3f minPos, Vector3f maxPos, float x, int normSign, float texCoordScale = 1.0f)
{
    poses.insert(poses.end(), Vector3f(x, minPos.y, minPos.z));
    poses.insert(poses.end(), Vector3f(x, minPos.y, maxPos.z));
    poses.insert(poses.end(), Vector3f(x, maxPos.y, minPos.z));
    poses.insert(poses.end(), Vector3f(x, maxPos.y, maxPos.z));

    int max = BasicMath::Max(0, normSign);
    indices.insert(indices.end(), startingIndex);
    indices.insert(indices.end(), startingIndex + 2 - normSign);
    indices.insert(indices.end(), startingIndex + 2 + normSign);
    indices.insert(indices.end(), startingIndex);
    indices.insert(indices.end(), startingIndex + 2 + max);
    indices.insert(indices.end(), startingIndex + 3 - max);

    startingIndex += 4;

    Vector3f norm(normSign, 0.0f, 0.0f);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);

    texCoords.insert(texCoords.end(), Vector2f(0.0f, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(0.0f, texCoordScale));
    texCoords.insert(texCoords.end(), Vector2f(texCoordScale, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(texCoordScale, texCoordScale));
}
//Creates a quad on the given y face of a cube bound by the given min/max points.
void CreateYAxisQuad(std::vector<Vector3f> & poses, std::vector<Vector3f> & normals, std::vector<Vector2f> & texCoords, std::vector<unsigned int> & indices,
                     unsigned int & startingIndex, Vector3f minPos, Vector3f maxPos, float y, int normSign, float texCoordScale = 1.0f)
{
    poses.insert(poses.end(), Vector3f(minPos.x, y, minPos.z));
    poses.insert(poses.end(), Vector3f(minPos.x, y, maxPos.z));
    poses.insert(poses.end(), Vector3f(maxPos.x, y, minPos.z));
    poses.insert(poses.end(), Vector3f(maxPos.x, y, maxPos.z));

    int max = BasicMath::Max(0, normSign);
    indices.insert(indices.end(), startingIndex);
    indices.insert(indices.end(), startingIndex + 2 + normSign);
    indices.insert(indices.end(), startingIndex + 2 - normSign);
    indices.insert(indices.end(), startingIndex);
    indices.insert(indices.end(), startingIndex + 3 - max);
    indices.insert(indices.end(), startingIndex + 2 + max);

    startingIndex += 4;

    Vector3f norm(0.0f, (float)normSign, 0.0f);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);

    texCoords.insert(texCoords.end(), Vector2f(0.0f, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(0.0f, texCoordScale));
    texCoords.insert(texCoords.end(), Vector2f(texCoordScale, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(texCoordScale, texCoordScale));
}
//Creates a quad on the given z face of a cube bound by the given min/max points.
void CreateZAxisQuad(std::vector<Vector3f> & poses, std::vector<Vector3f> & normals, std::vector<Vector2f> & texCoords, std::vector<unsigned int> & indices,
                     unsigned int & startingIndex, Vector3f minPos, Vector3f maxPos, float z, int normSign, float texCoordScale = 1.0f)
{
    poses.insert(poses.end(), Vector3f(minPos.x, minPos.y, z));
    poses.insert(poses.end(), Vector3f(minPos.x, maxPos.y, z));
    poses.insert(poses.end(), Vector3f(maxPos.x, minPos.y, z));
    poses.insert(poses.end(), Vector3f(maxPos.x, maxPos.y, z));

    int max = BasicMath::Max(0, normSign);
    indices.insert(indices.end(), startingIndex);
    indices.insert(indices.end(), startingIndex + 2 - normSign);
    indices.insert(indices.end(), startingIndex + 2 + normSign);
    indices.insert(indices.end(), startingIndex);
    indices.insert(indices.end(), startingIndex + 2 + max);
    indices.insert(indices.end(), startingIndex + 3 - max);

    startingIndex += 4;

    Vector3f norm(0.0f, 0.0f, normSign);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);
    normals.insert(normals.end(), norm);

    texCoords.insert(texCoords.end(), Vector2f(0.0f, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(0.0f, texCoordScale));
    texCoords.insert(texCoords.end(), Vector2f(texCoordScale, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(texCoordScale, texCoordScale));
}


void VC::BuildTriangles(std::vector<Vector3f> & vertices, std::vector<Vector3f> & normals, std::vector<Vector2f> & texCoords,
                        std::vector<unsigned int> & indices,
                        const VoxelChunk * beforeMinX, const VoxelChunk * afterMaxX,
                        const VoxelChunk * beforeMinY, const VoxelChunk * afterMaxY,
                        const VoxelChunk * beforeMinZ, const VoxelChunk * afterMaxZ) const
{
    if (IsEmpty()) return;
    if (IsFull())
    {
        unsigned int index = 0;

        Vector3f minWorld = LocalToWorldSpace(Vector3i()),
                 maxWorld = LocalToWorldSpace(Vector3i(ChunkSize, ChunkSize, ChunkSize));

        if (beforeMinX == 0 || !beforeMinX->IsFull())
        {
            CreateXAxisQuad(vertices, normals, texCoords, indices, index, minWorld, maxWorld, minWorld.x, -1, ChunkSizeF);
        }
        if (beforeMinY == 0 || !beforeMinY->IsFull())
        {
            CreateYAxisQuad(vertices, normals, texCoords, indices, index, minWorld, maxWorld, minWorld.y, -1, ChunkSizeF);
        }
        if (beforeMinZ == 0 || !beforeMinZ->IsFull())
        {
            CreateZAxisQuad(vertices, normals, texCoords, indices, index, minWorld, maxWorld, minWorld.z, -1, ChunkSizeF);
        }
        if (afterMaxX == 0 || !afterMaxX->IsFull())
        {
            CreateXAxisQuad(vertices, normals, texCoords, indices, index, minWorld, maxWorld, maxWorld.x, 1, ChunkSizeF);
        }
        if (afterMaxY == 0 || !afterMaxY->IsFull())
        {
            CreateYAxisQuad(vertices, normals, texCoords, indices, index, minWorld, maxWorld, maxWorld.y, 1, ChunkSizeF);
        }
        if (afterMaxZ == 0 || !afterMaxZ->IsFull())
        {
            CreateZAxisQuad(vertices, normals, texCoords, indices, index, minWorld, maxWorld, maxWorld.z, 1, ChunkSizeF);
        }


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
        return other->GetVoxelLocal(Vector3i(loc.x + ChunkSize, loc.y, loc.z));
    };

    if (beforeMinY == 0) gMinY = noVal;
    else gMinY = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x, loc.y + ChunkSize, loc.z));
    };

    if (beforeMinZ == 0) gMinZ = noVal;
    else gMinZ = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x, loc.y, loc.z + ChunkSize));
    };

    if (afterMaxX == 0) gMaxX = noVal;
    else gMaxX = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x - ChunkSize, loc.y, loc.z));
    };

    if (afterMaxY == 0) gMaxY = noVal;
    else gMaxY = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x, loc.y - ChunkSize, loc.z));
    };

    if (afterMaxZ == 0) gMaxZ = noVal;
    else gMaxZ = [](Vector3i loc, const VoxelChunk * other)
    {
        return other->GetVoxelLocal(Vector3i(loc.x, loc.y, loc.z - ChunkSize));
    };


    //For every empty voxel, create the triangles for any solid voxels touching it.

    Vector3i startLoc;
    Vector3f worldOffset = Vector3f(MinCorner.x, MinCorner.y, MinCorner.z);

    Vector3f voxelStart, voxelEnd;
    unsigned int startingIndex = 0;
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
                        CreateXAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, voxelStart.x, 1);
                    }
                    if ((yMin.y >= 0 && voxels[yMin]) ||
                        (yMin.y < 0 && gMinY(yMin, beforeMinY)))
                    {
                        CreateYAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, voxelStart.y, 1);
                    }
                    if ((zMin.z >= 0 && voxels[zMin]) ||
                        (zMin.z < 0 && gMinZ(zMin, beforeMinZ)))
                    {
                        CreateZAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, voxelStart.z, 1);
                    }
                    if ((xMax.x < ChunkSize && voxels[xMax]) ||
                        (xMax.x >= ChunkSize && gMaxX(xMax, afterMaxX)))
                    {
                        CreateXAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, voxelEnd.x, -1);
                    }
                    if ((yMax.y < ChunkSize && voxels[yMax]) ||
                        (yMax.y >= ChunkSize && gMaxY(yMax, afterMaxY)))
                    {
                        CreateYAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, voxelEnd.y, -1);
                    }
                    if ((zMax.z < ChunkSize && voxels[zMax]) ||
                        (zMax.z >= ChunkSize && gMaxZ(zMax, afterMaxZ)))
                    {
                        CreateZAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, voxelEnd.z, -1);
                    }
                }
            }
        }
    }


    //Build the outside edges.
    
    Vector3f chunkMin = LocalToWorldSpace(Vector3f()),
             chunkMax = LocalToWorldSpace(Vector3f(ChunkSizeF, ChunkSizeF, ChunkSizeF));

    //The Z edges.
    for (startLoc.z = 0; startLoc.z < ChunkSize; startLoc.z += ChunkSize - 1)
    {
        int sign = BasicMath::Sign(startLoc.z - (ChunkSizeF * 0.5f));

        voxelStart.z = (VoxelSizeF * startLoc.z) + worldOffset.z;
        voxelEnd.z = voxelStart.z + VoxelSizeF;

        for (startLoc.y = 0; startLoc.y < ChunkSize; startLoc.y++)
        {
            voxelStart.y = (VoxelSizeF * startLoc.y) + worldOffset.y;
            voxelEnd.y = voxelStart.y + VoxelSizeF;

            for (startLoc.x = 0; startLoc.x < ChunkSize; startLoc.x++)
            {
                voxelStart.x = (VoxelSizeF * startLoc.x) + worldOffset.x;
                voxelEnd.x = voxelStart.x + VoxelSizeF;

                if (GetVoxelLocal(startLoc) &&
                    ((startLoc.z == 0             && !gMinZ(Vector3i(startLoc.x, startLoc.y, startLoc.z - 1), beforeMinZ)) ||
                     (startLoc.z == ChunkSize - 1 && !gMaxZ(Vector3i(startLoc.x, startLoc.y, startLoc.z + 1), afterMaxZ))))
                {
                    float z = ((startLoc.z + BasicMath::Sign(startLoc.z)) * VoxelSizeF) + MinCorner.z;
                    CreateZAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, z, sign);

                }
            }
        }
    }

    //The Y edges.
    for (startLoc.y = 0; startLoc.y < ChunkSize; startLoc.y += ChunkSize - 1)
    {
        int sign = BasicMath::Sign(startLoc.y - (ChunkSizeF * 0.5f));

        voxelStart.y = (VoxelSizeF * startLoc.y) + worldOffset.y;
        voxelEnd.y = voxelStart.y + VoxelSizeF;

        for (startLoc.z = 0; startLoc.z < ChunkSize; startLoc.z++)
        {
            voxelStart.z = (VoxelSizeF * startLoc.z) + worldOffset.z;
            voxelEnd.z = voxelStart.z + VoxelSizeF;

            for (startLoc.x = 0; startLoc.x < ChunkSize; startLoc.x++)
            {
                voxelStart.x = (VoxelSizeF * startLoc.x) + worldOffset.x;
                voxelEnd.x = voxelStart.x + VoxelSizeF;

                if (GetVoxelLocal(startLoc) &&
                    ((startLoc.y == 0             && !gMinY(Vector3i(startLoc.x, startLoc.y - 1, startLoc.z), beforeMinY)) ||
                     (startLoc.y == ChunkSize - 1 && !gMaxY(Vector3i(startLoc.x, startLoc.y + 1, startLoc.z), afterMaxY))))
                {
                    float y = ((startLoc.y + BasicMath::Sign(startLoc.y)) * VoxelSizeF) + MinCorner.y;
                    CreateYAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, y, sign);
                }
            }
        }
    }

    //The X edges.
    for (startLoc.x = 0; startLoc.x < ChunkSize; startLoc.x += ChunkSize - 1)
    {
        int sign = BasicMath::Sign(startLoc.x - (ChunkSizeF * 0.5f));

        voxelStart.x = (VoxelSizeF * startLoc.x) + worldOffset.x;
        voxelEnd.x = voxelStart.x + VoxelSizeF;

        for (startLoc.z = 0; startLoc.z < ChunkSize; startLoc.z++)
        {
            voxelStart.z = (VoxelSizeF * startLoc.z) + worldOffset.z;
            voxelEnd.z = voxelStart.z + VoxelSizeF;

            for (startLoc.y = 0; startLoc.y < ChunkSize; startLoc.y++)
            {
                voxelStart.y = (VoxelSizeF * startLoc.y) + worldOffset.y;
                voxelEnd.y = voxelStart.y + VoxelSizeF;

                if (GetVoxelLocal(startLoc) &&
                    ((startLoc.x == 0             && !gMinX(Vector3i(startLoc.x - 1, startLoc.y, startLoc.z), beforeMinX)) ||
                     (startLoc.x == ChunkSize - 1 && !gMaxX(Vector3i(startLoc.x + 1, startLoc.y, startLoc.z), afterMaxX))))
                {
                    float x = ((startLoc.x + BasicMath::Sign(startLoc.x)) * VoxelSizeF) + MinCorner.x;
                    CreateXAxisQuad(vertices, normals, texCoords, indices, startingIndex, voxelStart, voxelEnd, x, sign);
                }
            }
        }
    }

    std::cout << "Vertices: " << vertices.size() << "; indices: " << indices.size() << "\n";
}