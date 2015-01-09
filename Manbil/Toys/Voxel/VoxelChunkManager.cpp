#include "VoxelChunkManager.h"

#include "../../Math/Higher Math/Geometryf.h"
#include "../../Math/Shapes/ThreeDShapes.h"
#include "../../DebugAssist.h"
#include <iostream>


typedef VoxelChunk VC;
typedef VoxelChunkManager VCM;

#define TS(x) (std::to_string(x))


VCM::RayCastResult VCM::CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist) const
{
    //Find the axis that moves the most, and figure out what
    //   't' increment is needed to move the ray one chunk length along that axis.
    unsigned int largestRayAxis = Geometryf::GetLongestAxis(rayDir);
    float destination = rayStart[largestRayAxis] + (Mathf::Sign(rayDir[largestRayAxis]) * (VC::ChunkSizeF * VC::VoxelSizeF));
    float chunkMoveLength = Geometryf::GetPointOnLineAtValue(rayStart, rayDir, largestRayAxis, destination).t;


    //March the ray forward in increments, checking each chunk it passes through.

    Vector3i oldChunkIndex = ToChunkIndex(rayStart);
    Vector3i newChunkIndex;
    Vector3i chunkCast;

    Vector3f moveIncrement = rayDir * chunkMoveLength;

    while ((GetChunk(oldChunkIndex) != 0 || GetChunk(newChunkIndex) != 0) && maxDist >= 0.0f)
    {
        newChunkIndex = ToChunkIndex(rayStart + moveIncrement);


        //Check every chunk that the ray intersected with
        //   (don't check the very last one, since that will be checked next iteration).
        RayCastResult castRes;

#pragma warning(disable: 4100)
        if (DoToEveryChunkPredicate([&castRes, rayStart, rayDir, maxDist, oldChunkIndex, newChunkIndex](Vector3i cIndex, VoxelChunk * cnk)
            {
                //If any voxels were hit, return the hit.
                VoxelChunk::VoxelRayHit tempChunk = cnk->CastRay(rayStart, rayDir, maxDist);
                if (tempChunk.CastResult.DidHitTarget)
                {
                    castRes = RayCastResult(cnk, tempChunk);
                    return true;
                }
                return false;
            }, oldChunkIndex, newChunkIndex))
        {
            return castRes;
        }
#pragma warning(default: 4100)

        rayStart += moveIncrement;
        maxDist -= chunkMoveLength;
        oldChunkIndex = newChunkIndex;
    }

    return RayCastResult();
}


//Gets whether ANY voxels with the majority of their volume inside the given Shape are solid.
bool VCM::GetAnyVoxels(const Shape & shpe) const
{
    VC::MinMaxI bounds = VoxelChunk::GetShapeBoundsI(shpe);

#pragma warning(disable: 4100)
    return DoToEveryChunkPredicate([&shpe, bounds](Vector3i lc, VoxelChunk * chnk)
    {
        Vector3i voxelIndexOffset = chnk->MinCorner / VC::VoxelSize;
        return chnk->DoToEveryVoxelPredicate([&shpe, chnk](Vector3u localVoxel)
        {
            return shpe.IsPointInside(chnk->LocalToWorldSpace(localVoxel)) &&
                   chnk->GetVoxelLocal(localVoxel);
        }, bounds.Min - voxelIndexOffset,
           bounds.Max - voxelIndexOffset);
    }, bounds.Min / VC::ChunkSize,
       bounds.Max / VC::ChunkSize);
#pragma warning(default: 4100)
}
//Gets whether ALL voxels with the majority of their volume inside the given Shape are solid.
bool VCM::GetAllVoxels(const Shape & shpe) const
{
    VC::MinMaxI bounds = VoxelChunk::GetShapeBoundsI(shpe);

#pragma warning(disable: 4100)
    return !DoToEveryChunkPredicate([&shpe, bounds](Vector3i lc, VoxelChunk * chnk)
    {
        Vector3i voxelIndexOffset = chnk->MinCorner / VC::VoxelSize;
        return !chnk->DoToEveryVoxelPredicate([&shpe, chnk](Vector3u localVoxel)
        {
            return shpe.IsPointInside(chnk->LocalToWorldSpace(localVoxel)) &&
                   !chnk->GetVoxelLocal(localVoxel);
        }, bounds.Min - voxelIndexOffset,
           bounds.Max - voxelIndexOffset);
    }, bounds.Min / VC::ChunkSize,
       bounds.Max / VC::ChunkSize);
#pragma warning(default: 4100)
}
//Toggles all voxels with the majority of their volume inside the given Shape.
void VCM::ToggleVoxels(const Shape & shpe)
{
    VC::MinMaxI bounds = VoxelChunk::GetShapeBoundsI(shpe);

#pragma warning(disable: 4100)
    DoToEveryChunk([&shpe, bounds](Vector3i lc, VoxelChunk * chnk)
    {
        Vector3i voxelIndexOffset = chnk->MinCorner / VC::VoxelSize;
        chnk->DoToEveryVoxel([&shpe, chnk](Vector3u localVoxel)
        {
            if (shpe.IsPointInside(chnk->LocalToWorldSpace(localVoxel)))
                chnk->ToggleVoxelLocal(localVoxel);
        }, bounds.Min - voxelIndexOffset,
           bounds.Max - voxelIndexOffset);
    }, bounds.Min / VC::ChunkSize,
       bounds.Max / VC::ChunkSize);
#pragma warning(default: 4100)
}
//Sets all voxels with the majority of their volume inside the given Shape.
void VCM::SetVoxels(const Shape & shpe, bool value)
{
    VC::MinMaxI bounds = VoxelChunk::GetShapeBoundsI(shpe);
    
#pragma warning(disable: 4100)
    DoToEveryChunk([&shpe, bounds, value](Vector3i lc, VoxelChunk * chnk)
    {
        Vector3i voxelIndexOffset = chnk->MinCorner / VC::VoxelSize;
        chnk->DoToEveryVoxel([&shpe, chnk, value](Vector3u localVoxel)
        {
            if (shpe.IsPointInside(chnk->LocalToWorldSpace(localVoxel)))
                chnk->SetVoxelLocal(localVoxel, value);
        }, bounds.Min - voxelIndexOffset,
           bounds.Max - voxelIndexOffset);
    }, bounds.Min / VC::ChunkSize,
       bounds.Max / VC::ChunkSize);
#pragma warning(default: 4100)
}


VCM::VoxelLocation VCM::GetOffset(VoxelLocation voxel, Vector3i face) const
{
    Vector3i added(voxel.LocalIndex.x + face.x, voxel.LocalIndex.y + face.y, voxel.LocalIndex.z + face.z);
    const int chunkSize = VoxelChunk::ChunkSize;

    Vector3i chunkIndex = GetChunkIndex(voxel.Chunk);
    while (added.x < 0)
    {
        chunkIndex = chunkIndex.LessX();
        added.x += chunkSize;
    }
    while (added.y < 0)
    {
        chunkIndex = chunkIndex.LessY();
        added.y += chunkSize;
    }
    while (added.z < 0)
    {
        chunkIndex = chunkIndex.LessZ();
        added.z += chunkSize;
    }
    while (added.x > chunkSize - 1)
    {
        chunkIndex = chunkIndex.MoreX();
        added.x -= chunkSize;
    }
    while (added.y > chunkSize - 1)
    {
        chunkIndex = chunkIndex.MoreY();
        added.y -= chunkSize;
    }
    while (added.z > chunkSize - 1)
    {
        chunkIndex = chunkIndex.MoreZ();
        added.z -= chunkSize;
    }
    voxel.LocalIndex = ToV3u(added);
    voxel.Chunk = GetChunk(chunkIndex);

    return voxel;
}