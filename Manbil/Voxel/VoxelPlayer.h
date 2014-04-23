#pragma once

#include "VoxelCamera.h"
#include "VoxelChunkManager.h"
#include "../Input/BoolInput.h"


//The player in a VoxelWorld instance.
class VoxelPlayer
{
public:

    VoxelChunkManager & Chunks;

    VoxelCamera Cam;
    Vector3f CamOffset;

    std::shared_ptr<Vector2Input> Movement;
    std::shared_ptr<BoolInput> Jump;

    float MoveSpeed;


    VoxelPlayer(VoxelChunkManager & chunks) : Chunks(chunks), MoveSpeed(10.0f) { }
    VoxelPlayer(Vector3f pos,
                VoxelChunkManager & chunks, VoxelCamera & cam,
                std::shared_ptr<Vector2Input> movement, std::shared_ptr<BoolInput> jump,
                Vector3f camOffset = Vector3f(0.0f, 0.0f, 0.5f), float moveSpeed = 2.5f)
        : Cam(cam), Chunks(chunks), Movement(movement), Jump(jump),
          CamOffset(camOffset), MoveSpeed(moveSpeed)
    {
        Cam.SetPosition(pos);
    }


    void Update(float elapsedTime, float totalTime);
};