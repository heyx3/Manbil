#include "VoxelCamera.h"

void VoxelCamera::Update(float elapsed, float total)
{
    RotationInput.Update(elapsed, total);

    if (false)//Oculus != 0 && Oculus->IsValid())
    {
        Quaternion rot;// = Oculus->GetRotation();
    
        SetRotation(Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
        AddYaw(RotationInput.EulerRotation.x);
        Rotate(rot);
    }
    else
    {
        SetRotation(Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
        AddPitch(RotationInput.EulerRotation.y);
        AddYaw(RotationInput.EulerRotation.z);
        AddRoll(RotationInput.EulerRotation.x);
    }
}