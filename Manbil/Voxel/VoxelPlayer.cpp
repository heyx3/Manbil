#include "VoxelPlayer.h"

void VoxelPlayer::Update(float el, float tot)
{
    Movement->Update(el, tot);
    Jump->Update(el, tot);

    Vector3f pos = Cam.GetPosition() - CamOffset;

    Vector2f movement(Movement->GetValue());
    movement *= el;

    Vector3f worldMoveForward = Cam.GetForward() * movement.y,
             worldMoveSideways = Cam.GetSideways() * movement.x;
    Cam.IncrementPosition((worldMoveForward + worldMoveSideways).Normalized() * MoveSpeed);
}