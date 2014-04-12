#pragma once

#include "../MovingCamera.h"
#include "../Input/LookRotation.h"


//A camera for the voxel world.
class VoxelCamera : public Camera
{
public:

    LookRotation RotationInput;
    sf::Window * Window;

    VoxelCamera(void)
        : RotationInput(std::shared_ptr<Vector2Input>()), Window(0)
    {

    }
    VoxelCamera(Vector3f startPos, const LookRotation & rotInput,
                Vector3f forward = Vector3f(1, 0, 0),
                Vector3f upward = Vector3f(0, 0, 1),
                sf::Window * window = 0)
        : Camera(startPos, forward, upward), RotationInput(rotInput), Window(window)
    {
        SetClosestDotVariance(0.0001f);
    }

    void Update(float elapsedTime, float totalElapsedTime);
};