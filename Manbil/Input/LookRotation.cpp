#include "LookRotation.h"


void LookRotation::Update(float el, float total)
{
    if (Input.get() == 0) return;


    Input->Update(el, total);

    Vector2f movement = Input->GetValue();
    movement.MultiplyComponents(Vector2f(RotateSpeed.z, RotateSpeed.y) * el);


    EulerRotation += Vector3f(0.0f, movement.y, movement.x);

    //Constrain rotation.
    EulerRotation.x = Mathf::Clamp(EulerRotation.x, -MaxRot.x, MaxRot.x);
    EulerRotation.y = Mathf::Clamp(EulerRotation.y, -MaxRot.y, MaxRot.y);
    EulerRotation.z = Mathf::Clamp(EulerRotation.z, -MaxRot.z, MaxRot.z);
}