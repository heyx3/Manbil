#pragma once

#include "Vector2Input.h"
#include "../Oculus/GeneralOculus.h"


//Manages input for rotating a camera.
//No input maps to rolling yet.
//"Forward" is X axis, "side" is Y axis, and "up" is Z axis.
class LookRotation
{
public:

    Vector3f EulerRotation, RotateSpeed, MaxRot;

    std::shared_ptr<Vector2Input> Input;

    LookRotation(std::shared_ptr<Vector2Input> input,
                 Vector3f rotSpeed = Vector3f(0.0f, 2.0f, 2.0f),
                 Vector3f maxRot = Vector3f(0.0f, BasicMath::DegToRad(85.0f), 9999.0f),
                 Vector3f startingRotation = Vector3f())
        : EulerRotation(startingRotation), RotateSpeed(rotSpeed), MaxRot(maxRot),
          Input(input)
    {
    }

    void Update(float elapsedTime, float totalTime);
};