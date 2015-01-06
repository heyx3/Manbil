#include "VoxelChunk.h"

#include "../../DebugAssist.h"
#include <assert.h>
#include <unordered_map>


typedef VoxelChunk VC;


const unsigned int VC::ChunkSize = 30;
const float VC::ChunkSizeF = 30.0f;

const unsigned int VC::VoxelSize = 3;
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

    Vector3u sMin = ToLocalVoxelIndex(bounds.GetMinCorner()),
             sMax = ToLocalVoxelIndex(bounds.GetMaxCorner());
    ret.Min = Vector3i(sMin.x, sMin.y, sMin.z);
    ret.Max = Vector3i(sMax.x, sMax.y, sMax.z);

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
    return DoToEveryVoxelPredicate([&shpe, thisVC](Vector3u lc) -> bool
    {
        return (shpe.IsPointInside(thisVC->LocalToWorldSpace(lc)) &&
                thisVC->GetVoxelLocal(lc));
    }, bounds.Min, bounds.Max);
}
bool VC::GetAllVoxels(const Shape & shpe) const
{
    MinMaxI bounds = GetLocalShapeBoundsI(shpe);

    const VC * thisVC = this;
    return !DoToEveryVoxelPredicate([&shpe, thisVC](Vector3u lc) -> bool
    {
        return (shpe.IsPointInside(thisVC->LocalToWorldSpace(lc)) &&
                !thisVC->GetVoxelLocal(lc));
    }, bounds.Min, bounds.Max);
}

void VC::ToggleVoxels(const Shape & shpe)
{
    MinMaxI bounds = GetLocalShapeBoundsI(shpe);

    VC * thisVC = this;
    DoToEveryVoxel([&shpe, thisVC](Vector3u lc)
    {
        if (shpe.IsPointInside(thisVC->LocalToWorldSpace(lc)))
            thisVC->ToggleVoxelLocal(lc);
    }, bounds.Min, bounds.Max);
}
void VC::SetVoxels(const Shape & shpe, bool value)
{
    MinMaxI bounds = GetLocalShapeBoundsI(shpe);

    VC * thisVC = this;
    DoToEveryVoxel([&shpe, thisVC, value](Vector3u lc)
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
                                                            rayStart[axis] + Mathf::Sign(rayDir[axis])).t;


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

        //Go through every voxel between the previous ray position and the current.
        //If any of them have a solid voxel, return that voxel's index.
        if (DoToEveryVoxelPredicate([&vrh, thisVC, worldRayStart, rayDir, lastRayPos, currentRayPos](Vector3u loc)
        {
            if (thisVC->GetVoxelLocal(loc))
            {
                Box3D vBounds = thisVC->GetBounds(loc);
                Shape::RayTraceResult res = Cube(vBounds).RayHitCheck(worldRayStart, rayDir);
                if (res.DidHitTarget)
                {
                    vrh.VoxelIndex = loc;
                    vrh.CastResult = res;
                    //Calculate which face was hit by finding the axis with the smallest difference between the hit pos and the bounds.
                    Vector3f distancesToMin(Mathf::Abs(res.HitPos.x - vBounds.GetXMin()),
                                            Mathf::Abs(res.HitPos.y - vBounds.GetYMin()),
                                            Mathf::Abs(res.HitPos.z - vBounds.GetZMin())),
                             distancesToMax(Mathf::Abs(res.HitPos.x - vBounds.GetXMax()),
                                            Mathf::Abs(res.HitPos.y - vBounds.GetYMax()),
                                            Mathf::Abs(res.HitPos.z - vBounds.GetZMax()));
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


void VC::BuildTriangles(std::vector<VoxelVertex> & vertices,
                        const VoxelChunk * beforeMinX, const VoxelChunk * afterMaxX,
                        const VoxelChunk * beforeMinY, const VoxelChunk * afterMaxY,
                        const VoxelChunk * beforeMinZ, const VoxelChunk * afterMaxZ) const
{
    if (IsEmpty()) return;

    const VoxelChunk * thisVC = this;

    //For every voxel, create a single point vertex.
    DoToEveryVoxel([thisVC, &vertices, beforeMinX, beforeMinY, beforeMinZ, afterMaxX, afterMaxY, afterMaxZ](Vector3u loc)
    {
        if (!thisVC->GetVoxelLocal(loc)) return;

        //Get whether each face is covered up.

        VoxelVertex vert;

        vert.MinExists.x = ((loc.x == 0 &&
                                (beforeMinX == 0 || !beforeMinX->GetVoxelLocal(Vector3u(ChunkSize - 1, loc.y, loc.z)))) ||
                            (loc.x > 0 && !thisVC->GetVoxelLocal(loc.LessX()))) ?
                                1.0f : 0.0f;

        vert.MinExists.y = ((loc.y == 0 &&
                                (beforeMinY == 0 || !beforeMinY->GetVoxelLocal(Vector3u(loc.x, ChunkSize - 1, loc.z)))) ||
                            (loc.y > 0 && !thisVC->GetVoxelLocal(loc.LessY()))) ? 1.0f : 0.0f;

        vert.MinExists.z = ((loc.z == 0 &&
                                (beforeMinZ == 0 || !beforeMinZ->GetVoxelLocal(Vector3u(loc.x, loc.y, ChunkSize - 1)))) ||
                            (loc.z > 0 && !thisVC->GetVoxelLocal(loc.LessZ()))) ? 1.0f : 0.0f;

        vert.MaxExists.x = ((loc.x == ChunkSize - 1 &&
                                (afterMaxX == 0 || !afterMaxX->GetVoxelLocal(Vector3u(0, loc.y, loc.z)))) ||
                            (loc.x < ChunkSize - 1 && !thisVC->GetVoxelLocal(loc.MoreX()))) ? 1.0f : 0.0f;

        vert.MaxExists.y = ((loc.y == ChunkSize - 1 &&
                                (afterMaxY == 0 || !afterMaxY->GetVoxelLocal(Vector3u(loc.x, 0, loc.z)))) ||
                            (loc.y < ChunkSize - 1 && !thisVC->GetVoxelLocal(loc.MoreY()))) ? 1.0f : 0.0f;

        vert.MaxExists.z = ((loc.z == ChunkSize - 1 &&
                                (afterMaxZ == 0 || !afterMaxZ->GetVoxelLocal(Vector3u(loc.x, loc.y, 0)))) ||
                            (loc.z < ChunkSize - 1 && !thisVC->GetVoxelLocal(loc.MoreZ()))) ? 1.0f : 0.0f;

        //If at least one face is visible, add the vertex to the buffer.
        if (vert.MinExists.LengthSquared() > 0.0f || vert.MaxExists.LengthSquared() > 0.0f)
        {
            const float halfVoxel = 0.5f * VoxelSizeF;
            vert.Pos = thisVC->LocalToWorldSpace(loc) + Vector3f(halfVoxel, halfVoxel, halfVoxel);
            vertices.insert(vertices.end(), vert);
        }
    });
}