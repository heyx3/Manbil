#pragma once

#include "../MovingCamera.h"
#include "../Input/LookRotation.h"
#include "../Oculus/OculusDevice.h"


//A camera for the voxel world.
class VoxelCamera : public Camera
{
public:

    LookRotation RotationInput;
    OculusDevice * Oculus;
    sf::Window * Window;

    VoxelCamera(void) : RotationInput(std::shared_ptr<Vector2Input>()), Window(0), Oculus(0) { }
    VoxelCamera(Vector3f startPos,
                const LookRotation & rotInput,
                OculusDevice * oculus,
                Vector3f forward = Vector3f(1, 0, 0),
                Vector3f upward = Vector3f(0, 0, 1),
                sf::Window * window = 0)
    : Camera(startPos, forward, upward), RotationInput(rotInput), Window(window), Oculus(oculus)
    {
        SetClosestDotVariance(-10.0001f);
    }

    void Update(float elapsedTime, float totalElapsedTime);

    //Gets the camera position plus the Oculus headset's position offset, if the device exists.
    Vector3f GetFinalPos(void) const
    {
        return GetPosition() +
               ((Oculus != 0 && Oculus->IsValid()) ? Oculus->GetPosition() : Vector3f());
    }
};