#pragma once

#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "Math/Higher Math/Camera.h"


//PRIORITY: Pull out mouse position calculation and mouse capturing into SFMLWorld.
//A Camera that is controlled with WASD/EQ and mouse.
//Spacebar toggles mouse capture/input.
class MovingCamera : public Camera
{
public:

	sf::Window * Window;

	MovingCamera(float moveSpd = 10.0f, float rSpeed = 0.03f)
        : pressedSpace(false), moveSpeed(moveSpd), rotSpeed(rSpeed)
    {
        Window = 0;
        mouseTarget = Vector2i(100, 100);
    }
	MovingCamera(Vector3f pos, float moveSpd = 10.0f, float rSpeed = 0.03f,
				 Vector3f forward = Vector3f(1, 0, 0), Vector3f up = Vector3f(0, 0, 1),
                 bool lockUp = true)
				 : Camera(pos, forward, up, lockUp), pressedSpace(false), moveSpeed(moveSpd), rotSpeed(rSpeed)
    {
        Window = 0;
        mouseTarget = Vector2i(100, 100);
    }

	float GetMoveSpeed(void) const { return moveSpeed; }
	float GetRotSpeed(void) const { return rotSpeed; }

	void SetMoveSpeed(float newVal) { moveSpeed = newVal; }
	void SetRotSpeed(float newVal) { rotSpeed = newVal; }

    //Updates this camera and returns whether the user pressed escape.
	virtual bool Update(float elapsedTime);

private:

	float moveSpeed, rotSpeed;
	Vector2i mouseTarget;

	bool pressedSpace, capMouse;

	sf::Vector2i Conv(Vector2i i) const { return sf::Vector2i(i.x, i.y); }
	Vector2i Conv(sf::Vector2i i) const { return Vector2i(i.x, i.y); }
};