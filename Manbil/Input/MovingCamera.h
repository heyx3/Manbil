#pragma once

#include "SFML/System.hpp"
#include "SFML/Window.hpp"

#include "../Math/Higher Math/Camera.h"


//A simple Camera that is controlled with WASD/EQ and mouse.
//Spacebar toggles mouse capture/input.
class MovingCamera : public Camera
{
public:

    //The key that toggles mouse capture.
    sf::Keyboard::Key ToggleMouseCapKey = sf::Keyboard::Space;

    //If set to a non-null value, this camera will read the mouse position
    //    and rotate itself accordingly.
	sf::Window* Window = 0;

    //The movement speed of this camera.
    float MoveSpeed;
    //The rotation speed of this camera.
    float RotSpeed;


	MovingCamera(float moveSpd = 10.0f, float rSpeed = 0.03f)
        : pressedSpace(false), MoveSpeed(moveSpd), RotSpeed(rSpeed),
          mouseTarget(100, 100) { }
	MovingCamera(Vector3f pos, float moveSpd = 10.0f, float rSpeed = 0.03f,
				 Vector3f forward = Vector3f(1, 0, 0), Vector3f up = Vector3f(0, 0, 1),
                 bool lockUp = true)
		: Camera(pos, forward, up, lockUp), pressedSpace(false),
          MoveSpeed(moveSpd), RotSpeed(rSpeed),
          mouseTarget(100, 100) { }


    bool IsMouseCapped(void) const { return capMouse; }


    //Updates this camera.
	virtual void Update(float elapsedTime);

private:

	Vector2i mouseTarget;

	bool pressedSpace, capMouse;

	sf::Vector2i Conv(Vector2i i) const { return sf::Vector2i(i.x, i.y); }
	Vector2i Conv(sf::Vector2i i) const { return Vector2i(i.x, i.y); }
};