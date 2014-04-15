#include "VoxelChunkManager.h"

#include "../Math/Higher Math/GeometricMath.h"
#include "../Math/Shapes/ThreeDShapes.h"

typedef VoxelChunk VC;
typedef VoxelChunkManager VCM;


VCM::RayCastResult VCM::CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist) const
{
    //Find the axis that moves the most, and figure out what
    //   't' increment is needed to move the ray one chunk length along that axis.
    unsigned int largestRayAxis = GeometricMath::GetLongestAxis(rayDir);
    float destination = rayStart[largestRayAxis] + (BasicMath::Sign(rayDir[largestRayAxis]) * (VC::ChunkSizeF * VC::VoxelSizeF));
    float chunkMoveLength = GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, largestRayAxis, destination).t;


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
        if (DoToEveryChunkPredicate([&castRes, rayStart, rayDir, maxDist](Vector3i cIndex, VoxelChunk * cnk)
        {
            //Only do any testing if the ray actually hit the chunk.
            if (Cube(cnk->GetBounds()).RayHitCheck(rayStart, rayDir).DidHitTarget)
            {
                //If any voxels were hit, return the hit.
                VoxelChunk::VoxelRayHit tempChunk = cnk->CastRay(rayStart, rayDir, maxDist);
                if (tempChunk.CastResult.DidHitTarget)
                {
                    castRes = RayCastResult(cnk, tempChunk);
                    return true;
                }
            }
            return false;
        }, oldChunkIndex, newChunkIndex))
            return castRes;

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

    return DoToEveryChunkPredicate([&shpe, bounds](Vector3i lc, VoxelChunk * chnk)
    {
        Vector3i voxelIndexOffset = chnk->MinCorner / VC::VoxelSize;
        return chnk->DoToEveryVoxelPredicate([&shpe, chnk](Vector3i localVoxel)
        {
            return shpe.IsPointInside(chnk->LocalToWorldSpace(localVoxel)) &&
                   chnk->GetVoxelLocal(localVoxel);
        }, bounds.Min - voxelIndexOffset,
           bounds.Max - voxelIndexOffset);
    }, bounds.Min / VC::ChunkSize,
       bounds.Max / VC::ChunkSize);
}
//Gets whether ALL voxels with the majority of their volume inside the given Shape are solid.
bool VCM::GetAllVoxels(const Shape & shpe) const
{
    VC::MinMaxI bounds = VoxelChunk::GetShapeBoundsI(shpe);

    return !DoToEveryChunkPredicate([&shpe, bounds](Vector3i lc, VoxelChunk * chnk)
    {
        Vector3i voxelIndexOffset = chnk->MinCorner / VC::VoxelSize;
        return !chnk->DoToEveryVoxelPredicate([&shpe, chnk](Vector3i localVoxel)
        {
            return shpe.IsPointInside(chnk->LocalToWorldSpace(localVoxel)) &&
                   !chnk->GetVoxelLocal(localVoxel);
        }, bounds.Min - voxelIndexOffset,
           bounds.Max - voxelIndexOffset);
    }, bounds.Min / VC::ChunkSize,
       bounds.Max / VC::ChunkSize);
}
//Toggles all voxels with the majority of their volume inside the given Shape.
void VCM::ToggleVoxels(const Shape & shpe)
{
    VC::MinMaxI bounds = VoxelChunk::GetShapeBoundsI(shpe);

    DoToEveryChunk([&shpe, bounds](Vector3i lc, VoxelChunk * chnk)
    {
        Vector3i voxelIndexOffset = chnk->MinCorner / VC::VoxelSize;
        chnk->DoToEveryVoxel([&shpe, chnk](Vector3i localVoxel)
        {
            if (shpe.IsPointInside(chnk->LocalToWorldSpace(localVoxel)))
                chnk->ToggleVoxelLocal(localVoxel);
        }, bounds.Min - voxelIndexOffset,
           bounds.Max - voxelIndexOffset);
    }, bounds.Min / VC::ChunkSize,
       bounds.Max / VC::ChunkSize);
}
//Sets all voxels with the majority of their volume inside the given Shape.
void VCM::SetVoxels(const Shape & shpe, bool value)
{
    VC::MinMaxI bounds = VoxelChunk::GetShapeBoundsI(shpe);

    DoToEveryChunk([&shpe, bounds, value](Vector3i lc, VoxelChunk * chnk)
    {
        Vector3i voxelIndexOffset = chnk->MinCorner / VC::VoxelSize;
        chnk->DoToEveryVoxel([&shpe, chnk, value](Vector3i localVoxel)
        {
            if (shpe.IsPointInside(chnk->LocalToWorldSpace(localVoxel)))
                chnk->SetVoxelLocal(localVoxel, value);
        }, bounds.Min - voxelIndexOffset,
           bounds.Max - voxelIndexOffset);
    }, bounds.Min / VC::ChunkSize,
       bounds.Max / VC::ChunkSize);
}