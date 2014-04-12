#include "VoxelPlayer.h"

void VoxelPlayer::Update(float el, float tot)
{
    Cam.Update(el, tot);

    Movement->Update(el, tot);
    Jump->Update(el, tot);

    Vector3f pos = Cam.GetPosition() - CamOffset;

    Vector2f movement(Movement->GetValue());

    Vector3f worldMoveForward = Cam.GetForward() * movement.y,
             worldMoveSideways = Cam.GetSideways() * movement.x,
             finalMove = worldMoveForward + worldMoveSideways;

    //finalMove.z = 0.0f;
    float len = finalMove.Length();
    if (len > 0.0f)
    {
        Cam.IncrementPosition(finalMove * el * MoveSpeed / len);
    }
}