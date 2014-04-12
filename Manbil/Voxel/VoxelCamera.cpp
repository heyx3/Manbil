#include "VoxelCamera.h"

void VoxelCamera::Update(float elapsed, float total)
{
    RotationInput.Update(elapsed, total);

    SetRotation(Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), true);
    AddPitch(RotationInput.EulerRotation.y);
    AddYaw(RotationInput.EulerRotation.z);
    AddRoll(RotationInput.EulerRotation.x);
}