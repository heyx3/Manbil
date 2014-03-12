#pragma once

#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "Math/Higher Math/Camera.h"

class MovingCamera : public Camera
{
public:

	sf::Window * Window;

	MovingCamera(float moveSpd = 10.0f, float rSpeed = 0.03f) : pressedSpace(false), moveSpeed(moveSpd), rotSpeed(rSpeed) { Window = NULL; mouseTarget = Vector2i(100, 100); }
	MovingCamera(Vector3f pos, float moveSpd = 10.0f, float rSpeed = 0.03f,
				 Vector3f forward = Vector3f(1, 0, 0), Vector3f up = Vector3f(0, 0, 1))
				 : Camera(pos, forward, up), pressedSpace(false), moveSpeed(moveSpd), rotSpeed(rSpeed) { Window = NULL; mouseTarget = Vector2i(100, 100); }

	float GetMoveSpeed(void) const { return moveSpeed; }
	float GetRotSpeed(void) const { return rotSpeed; }

	void SetMoveSpeed(float newVal) { moveSpeed = newVal; }
	void SetRotSpeed(float newVal) { rotSpeed = newVal; }

	bool Update(float elapsedTime);

private:

	float moveSpeed, rotSpeed;
	Vector2i mouseTarget;

	bool pressedSpace, capMouse;

	sf::Vector2i Conv(Vector2i i) const { return sf::Vector2i(i.x, i.y); }
	Vector2i Conv(sf::Vector2i i) const { return Vector2i(i.x, i.y); }
};