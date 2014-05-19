#include "VoxelChunk.h"

#include "../DebugAssist.h"
#include <assert.h>
#include <iostream>
#include <unordered_map>


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
    std::string dOutput;
    DebugAssist::STR.clear();
    //TODO: Remove debug printing from this and VoxelChunkManager::CastRay().


    VoxelRayHit vrh;

    Vector3f worldRayStart = rayStart;
    rayStart = ToLocalChunkSpace(rayStart);
    maxDist /= VoxelSizeF;

    dOutput += "\t\t\tLocal-chunk-space ray start: " + DebugAssist::ToString(rayStart) + "\n";

    //If the ray isn't even pointing towards the chunk, don't bother raycasting.
    if (IsEmpty() ||
        (rayStart.x < 0.0f && rayDir.x <= 0.0f) ||
        (rayStart.y < 0.0f && rayDir.y <= 0.0f) ||
        (rayStart.z < 0.0f && rayDir.z <= 0.0f) ||
        (rayStart.x >= ChunkSizeF && rayDir.x >= 0.0f) ||
        (rayStart.y >= ChunkSizeF && rayDir.y >= 0.0f) ||
        (rayStart.z >= ChunkSizeF && rayDir.z >= 0.0f))
    {
        dOutput += (IsEmpty() ?
                        "\t\t\tChunk is empty." :
                        "\t\t\tRay was outside chunk area. Voxel-space ray start: " +
                            DebugAssist::ToString(rayStart) + "\n\n");
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

    Vector3f initialRayStart = rayStart;

    while (distTraveled - 0.5f <= maxDist &&
           (!isXTooFar(rayStart.x) || !isYTooFar(rayStart.y) || !isZTooFar(rayStart.z)))
    {
        currentRayPos = rayStart.Floored();

        dOutput += "\t\t\tIterating from " + DebugAssist::ToString(lastRayPos) + " to " + DebugAssist::ToString(currentRayPos) + "\n";

        //Go through every voxel between the previous ray position and the current.
        //If any of them have a solid voxel, return that voxel's index.
        if (DoToEveryVoxelPredicate([&vrh, thisVC, worldRayStart, rayDir, lastRayPos, currentRayPos, &dOutput](Vector3i loc)
        {
            dOutput += "\t\t\t\tChecking local voxel " + DebugAssist::ToString(loc) + "\n";
            if (thisVC->GetVoxelLocal(loc))
            {
                Box3D vBounds = thisVC->GetBounds(loc);
                Shape::RayTraceResult res = Cube(vBounds).RayHitCheck(worldRayStart, rayDir);
                if (res.DidHitTarget)
                {
                    dOutput += "\t\t\t\t\tHit voxel. Hit: " + DebugAssist::ToString(res) + "; local voxel iteration: [" + DebugAssist::ToString(lastRayPos) + ", " + DebugAssist::ToString(currentRayPos) + "]\n";
                    vrh.VoxelIndex = loc;
                    vrh.CastResult = res;
                    //Calculate which face was hit by finding the axis with the smallest difference between the hit pos and the bounds.
                    Vector3f distancesToMin(BasicMath::Abs(res.HitPos.x - vBounds.GetXMin()),
                                            BasicMath::Abs(res.HitPos.y - vBounds.GetYMin()),
                                            BasicMath::Abs(res.HitPos.z - vBounds.GetZMin())),
                             distancesToMax(BasicMath::Abs(res.HitPos.x - vBounds.GetXMax()),
                                            BasicMath::Abs(res.HitPos.y - vBounds.GetYMax()),
                                            BasicMath::Abs(res.HitPos.z - vBounds.GetZMax()));
                    bool minX_minY = (distancesToMin.x < distancesToMin.y),
                         minX_minZ = (distancesToMin.x < distancesToMin.z),
                         minX_maxX = (distancesToMin.x < distancesToMax.x),
                         minX_maxY = (distancesToMin.x < distancesToMax.y),
                         minX_maxZ = (distancesToMin.x < distancesToMax.z),
                         minY_minZ = (distancesToMin.y < distancesToMin.z),
                         minY_maxX = (distancesToMin.y < distancesToMax.x),
                         minY_maxY = (distancesToMin.y < distancesToMax.y),
                         minY_maxZ = (distancesToMin.y < distancesToMax.z),
                         minZ_maxX = (distancesToMin.z < distancesToMax.x),
                         minZ_maxY = (distancesToMin.z < distancesToMax.y),
                         minZ_maxZ = (distancesToMin.z < distancesToMax.z),
                         maxX_maxY = (distancesToMax.x < distancesToMax.y),
                         maxX_maxZ = (distancesToMax.x < distancesToMax.z),
                         maxY_maxZ = (distancesToMax.y < distancesToMax.z);
                    if (minX_minY && minX_minZ && minX_maxX && minX_maxY && minX_maxZ)
                        vrh.Face.x = -1;
                    else if (!minX_minY && minY_minZ && minY_maxX && minY_maxY && minY_maxZ)
                        vrh.Face.y = -1;
                    else if (!minX_minZ && !minY_minZ && minZ_maxX && minZ_maxY && minZ_maxZ)
                        vrh.Face.z = -1;
                    else if (!minX_maxX && !minY_maxX && !minZ_maxX && maxX_maxY && maxX_maxZ)
                        vrh.Face.x = 1;
                    else if (!minX_maxY && !minY_maxY && !minZ_maxY && !maxX_maxY && maxY_maxZ)
                        vrh.Face.y = 1;
                    else
                    {
                        assert(!minX_maxZ && !minY_maxZ && !minZ_maxZ && !maxX_maxZ && !maxY_maxZ);
                        vrh.Face.z = 1;
                    }
                    dOutput += "\t\t\t\t\tFace: " + DebugAssist::ToString(vrh.Face) + "\n";
                    return true;
                }
            }
            dOutput += "\t\t\t\t\tNo hit.\n\n";
            return false;
        }, lastRayPos, currentRayPos))
            break;

        lastRayPos = currentRayPos;
        rayStart += moveIncrement;
        worldRayStart += moveIncrement;
        distTraveled += tIncrement;
    }
    dOutput += "\t\t\tDone chunk ray trace.\n\n";
    DebugAssist::STR = dOutput;
    return vrh;
}



//Creates a quad on the given x face of a cube bound by the given min/max points.
void CreateXAxisQuad(std::vector<Vector3f> & poses, std::vector<Vector3f> & normals, std::vector<Vector2f> & texCoords, std::vector<unsigned int> & indices,
                     unsigned int & startingIndex, Vector3f minPos, Vector3f maxPos, float x, int normSign,
                     float yTexCoordScale = 1.0f, float zTexCoordScale = 1.0f)
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
    texCoords.insert(texCoords.end(), Vector2f(0.0f, zTexCoordScale));
    texCoords.insert(texCoords.end(), Vector2f(yTexCoordScale, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(yTexCoordScale, zTexCoordScale));
}
//Creates a quad on the given y face of a cube bound by the given min/max points.
void CreateYAxisQuad(std::vector<Vector3f> & poses, std::vector<Vector3f> & normals, std::vector<Vector2f> & texCoords, std::vector<unsigned int> & indices,
                     unsigned int & startingIndex, Vector3f minPos, Vector3f maxPos, float y, int normSign,
                     float xTexCoordScale = 1.0f, float zTexCoordScale = 1.0f)
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
    texCoords.insert(texCoords.end(), Vector2f(0.0f, zTexCoordScale));
    texCoords.insert(texCoords.end(), Vector2f(xTexCoordScale, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(xTexCoordScale, zTexCoordScale));
}
//Creates a quad on the given z face of a cube bound by the given min/max points.
void CreateZAxisQuad(std::vector<Vector3f> & poses, std::vector<Vector3f> & normals, std::vector<Vector2f> & texCoords, std::vector<unsigned int> & indices,
                     unsigned int & startingIndex, Vector3f minPos, Vector3f maxPos, float z, int normSign,
                     float xTexCoordScale = 1.0f, float yTexCoordScale = 1.0f)
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
    texCoords.insert(texCoords.end(), Vector2f(0.0f, yTexCoordScale));
    texCoords.insert(texCoords.end(), Vector2f(xTexCoordScale, 0.0f));
    texCoords.insert(texCoords.end(), Vector2f(xTexCoordScale, yTexCoordScale));
}


void VC::BuildTriangles(std::vector<Vector3f> & vertices, std::vector<Vector3f> & normals, std::vector<Vector2f> & texCoords,
                        std::vector<unsigned int> & indices,
                        const VoxelChunk * beforeMinX, const VoxelChunk * afterMaxX,
                        const VoxelChunk * beforeMinY, const VoxelChunk * afterMaxY,
                        const VoxelChunk * beforeMinZ, const VoxelChunk * afterMaxZ) const
{
    if (IsEmpty()) return;

    //Build cubes (using a simple naive algorithm), and create 6 quads for each face of those cubes.

    //TODO: Instead of pushing out one axis at a time, try pushing as many axes out as possible.

    //Keep track of which locations are already part of a cube.
    Array3D<bool> usedLocations(ChunkSize, ChunkSize, ChunkSize, false);

    const VoxelChunk * vc = this;
    unsigned int currentIndex = 0;
    DoToEveryVoxel([&usedLocations, vc, &vertices, &normals, &texCoords, &indices, &currentIndex](Vector3i loc)
    {
        //If this voxel is solid and not already part of a cube, build a new cube.
        if (!usedLocations[loc] && vc->GetVoxelLocal(loc))
        {
            usedLocations[loc] = true;
            Vector3i endLoc = loc;

            //First push the cube out along the X.
            while (true)
            {
                //If we've reached the end of the chunk, or the next voxel is already in a cube, or the next voxel is empty, stop pushing.
                if (endLoc.x >= ChunkSize - 1 || usedLocations[endLoc.MoreX()] || !vc->GetVoxelLocal(endLoc.MoreX()))
                    break;

                endLoc.x += 1;
                usedLocations[endLoc] = true;
            }

            //Next, push the cube out along the Y.
            while (true)
            {
                //If we've reached the end of the chunk, stop pushing.
                if (endLoc.y >= ChunkSize - 1)
                    break;
                //If one of the next line of voxels is already in a cube, or one of the next line of voxels is empty, stop pushing.
                bool exit = false;
                for (Vector3i testLoc(loc.x, endLoc.y + 1, endLoc.z); !exit && testLoc.x <= endLoc.x; ++testLoc.x)
                    if (usedLocations[testLoc] || !vc->GetVoxelLocal(testLoc))
                        exit = true;
                if (exit) break;

                endLoc.y += 1;
                for (Vector3i newLoc(loc.x, endLoc.y, endLoc.z); newLoc.x <= endLoc.x; ++newLoc.x)
                    usedLocations[newLoc] = true;
            }

            //Finally, push the cube out along the Z.
            while (true)
            {
                //If we've reached the end of the chunk, stop pushing.
                if (endLoc.z >= ChunkSize - 1)
                    break;
                //If one of the next region of voxels is already in a cube, or one of the next region of voxels is empty, stop pushing.
                bool exit = false;
                for (Vector3i testLoc(loc.x, loc.y, endLoc.z + 1); !exit && testLoc.y <= endLoc.y; ++testLoc.y)
                    for (testLoc.x = loc.x; !exit && testLoc.x <= endLoc.x; ++testLoc.x)
                        if (usedLocations[testLoc] || !vc->GetVoxelLocal(testLoc))
                            exit = true;
                if (exit) break;

                endLoc.z += 1;
                for (Vector3i newLoc(loc.x, loc.y, endLoc.z); newLoc.y <= endLoc.y; ++newLoc.y)
                    for (newLoc.x = loc.x; newLoc.x <= endLoc.x; ++newLoc.x)
                        usedLocations[newLoc] = true;
            }

            //Now that we have the cube, create the vertices for it.
            Vector3f minPos = vc->LocalToWorldSpace(loc),
                     maxPos = vc->LocalToWorldSpace(endLoc + Vector3i(1, 1, 1)),
                     texCoordScale(endLoc.x - loc.x + 1, endLoc.y - loc.y + 1, endLoc.z - loc.z + 1);

            //See if any faces are fully covered by voxels. If they are, don't create quads for them.

            bool fullLessX = true;
            if (loc.x > 0)
                for (Vector3i testLoc(loc.x - 1, loc.y, loc.z); fullLessX && testLoc.z <= endLoc.z; ++testLoc.z)
                    for (testLoc.y = loc.y; fullLessX && testLoc.y <= endLoc.y; ++testLoc.y)
                        fullLessX = fullLessX && vc->GetVoxelLocal(testLoc);

            bool fullLessY = true;
            if (loc.y > 0)
                for (Vector3i testLoc(loc.x, loc.y - 1, loc.z); fullLessY && testLoc.z <= endLoc.z; ++testLoc.z)
                    for (testLoc.x = loc.x; fullLessY && testLoc.x <= endLoc.x; ++testLoc.x)
                        fullLessY = fullLessY && vc->GetVoxelLocal(testLoc);

            bool fullLessZ = true;
            if (loc.z > 0)
                for (Vector3i testLoc(loc.x, loc.y, loc.z - 1); fullLessZ && testLoc.y <= endLoc.y; ++testLoc.y)
                    for (testLoc.x = loc.x; fullLessZ && testLoc.x <= endLoc.x; ++testLoc.x)
                        fullLessZ = fullLessZ && vc->GetVoxelLocal(testLoc);

            bool fullMoreX = true;
            if (endLoc.x < ChunkSize - 1)
                for (Vector3i testLoc(endLoc.x + 1, loc.y, loc.z); fullMoreX && testLoc.z <= endLoc.z; ++testLoc.z)
                    for (testLoc.y = loc.y; fullMoreX && testLoc.y <= endLoc.y; ++testLoc.y)
                        fullMoreX = fullMoreX && vc->GetVoxelLocal(testLoc);

            bool fullMoreY = true;
            if (endLoc.y < ChunkSize - 1)
                for (Vector3i testLoc(loc.x, endLoc.y + 1, loc.z); fullMoreY && testLoc.z <= endLoc.z; ++testLoc.z)
                    for (testLoc.x = loc.x; fullMoreY && testLoc.x <= endLoc.x; ++testLoc.x)
                        fullMoreY = fullMoreY && vc->GetVoxelLocal(testLoc);

            bool fullMoreZ = true;
            if (endLoc.z < ChunkSize - 1)
                for (Vector3i testLoc(loc.x, loc.y, endLoc.z + 1); fullMoreZ && testLoc.y <= endLoc.y; ++testLoc.y)
                    for (testLoc.x = loc.x; fullMoreZ && testLoc.x <= endLoc.x; ++testLoc.x)
                        fullMoreZ = fullMoreZ && vc->GetVoxelLocal(testLoc);


            if (loc.x == 0 || !fullLessX)
                CreateXAxisQuad(vertices, normals, texCoords, indices, currentIndex, minPos, maxPos, minPos.x, -1, texCoordScale.y, texCoordScale.z);
            if (endLoc.x == ChunkSize - 1 || !fullMoreX)
                CreateXAxisQuad(vertices, normals, texCoords, indices, currentIndex, minPos, maxPos, maxPos.x, 1, texCoordScale.y, texCoordScale.z);

            if (loc.y == 0 || !fullLessY)
                CreateYAxisQuad(vertices, normals, texCoords, indices, currentIndex, minPos, maxPos, minPos.y, -1, texCoordScale.x, texCoordScale.z);
            if (endLoc.y == ChunkSize - 1 || !fullMoreY)
                CreateYAxisQuad(vertices, normals, texCoords, indices, currentIndex, minPos, maxPos, maxPos.y, 1, texCoordScale.x, texCoordScale.z);

            if (loc.z == 0 || !fullLessZ)
                CreateZAxisQuad(vertices, normals, texCoords, indices, currentIndex, minPos, maxPos, minPos.z, -1, texCoordScale.x, texCoordScale.y);
            if (endLoc.z == ChunkSize - 1 || !fullMoreZ)
                CreateZAxisQuad(vertices, normals, texCoords, indices, currentIndex, minPos, maxPos, maxPos.z, 1, texCoordScale.x, texCoordScale.y);
        }
    });

    std::cout << "Vertices: " << vertices.size() << "; indices: " << indices.size() << "\n";
}