#include "VoxelChunkManager.h"

#include "../Math/Higher Math/GeometricMath.h"

typedef VoxelChunk VC;
typedef VoxelChunkManager VCM;


VCM::RayCastResult VCM::CastRay(Vector3f rayStart, Vector3f rayDir, float maxDist) const
{
    //Find the axis that moves the most, and figure out what
    //   't' increment is needed to move the ray one chunk length along that axis.
    unsigned int largestRayAxis = GeometricMath::GetLongestAxis(rayDir);
    float destination = rayDir[largestRayAxis] + (VC::ChunkSizeF * VC::VoxelSizeF);
    float chunkMoveLength = GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, largestRayAxis, destination).t;


    //March the ray forward in increments, checking each chunk it passes through.

    Vector3i oldChunkIndex = ToChunkIndex(rayStart);
    Vector3i newChunkIndex;
    VoxelChunk * rayInside = GetChunk(oldChunkIndex);
    Vector3i chunkCast;

    Vector3f moveIncrement = rayDir * chunkMoveLength;
    rayStart += moveIncrement;
    maxDist -= chunkMoveLength;

    while (rayInside != 0 && maxDist >= 0.0f)
    {
        newChunkIndex = ToChunkIndex(rayStart);

        //Check every chunk that the ray intersected with
        //   (don't check the very last one, since that will be checked next iteration).
        Vector3i tempChunk;
        for (tempChunk.z = oldChunkIndex.z; tempChunk.z <= newChunkIndex.z; tempChunk.z++)
            for (tempChunk.y = oldChunkIndex.y; tempChunk.y <= newChunkIndex.y; tempChunk.y++)
                for (tempChunk.x = oldChunkIndex.x; tempChunk.x <= newChunkIndex.x; tempChunk.x++)
                {
                    if (tempChunk != newChunkIndex)
                    {
                        Vector3i chunkCast = rayInside->CastRay(rayStart, rayDir, maxDist);
                        if (chunkCast.x != -1)
                            return RayCastResult(rayInside, chunkCast);
                    }
                }
    }

    return RayCastResult();





    /*
    Vector3i oldChunkIndex = ToChunkIndex(rayStart),
             newChunkIndex;
    Vector3i rayHit;
    VoxelChunk * rayInside = GetChunk(oldChunkIndex);
    float movedDist = 0.0f;

    //Find the axis that moves the most, and figure out what
    //   't' increment is needed to move the ray one chunk length along that axis.

    unsigned int largestRayAxis = GeometricMath::GetLongestAxis(rayDir);
    float chunkMoveLength = GeometricMath::GetPointOnLineAtValue(rayStart, rayDir, largestRayAxis,
                                                                 rayDir[largestRayAxis] + (VC::ChunkSizeF * VC::VoxelSizeF)).t;
    Vector3f rayMoveIncrement = rayDir * chunkMoveLength;

    while (rayInside != 0 && movedDist <= maxDist)
    {
        //newChunkIndex = 

        rayHit = rayInside->CastRay(rayStart, rayDir, maxDist);
        if (rayHit.x != -1) return RayCastResult(rayInside, rayHit);
        
        movedDist += chunkMoveLength;
        rayStart += rayMoveIncrement;
        
        //rayInside = GetChunk()
    }

    return RayCastResult();
    */
}