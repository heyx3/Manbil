#pragma once

#include "../Vector2Input.h"
#include "../../Interval.h"
#include <SFML/Window/Mouse.hpp>

//Gets the delta position of the mouse per second.
//Optionally resets the mouse to a given position every time this input is updated.
class MouseDeltaVector2Input : Vector2Input
{
public:

	//The maximum possible mouse delta per second.
	float MaxSpeed;
	//If X or Y is set to -1, this field is not used.
	//Otherwise, sets the mouse to this position every time GetRawInput() is called.
	sf::Vector2i MouseResetPos;


	MouseDeltaVector2Input(float maxSpeed, DeadzonePtr deadzone,
						   sf::Vector2i mouseResetPos = sf::Vector2i(-1, -1), Vector2f initialPos = Vector2f())
		: Vector2Input(deadzone, Vector2f()), MaxSpeed(maxSpeed), MouseResetPos(mouseResetPos), prevPos(mouseResetPos), prevTotalTime(-1.0f)
	{

	}

protected:

	virtual Vector2f GetRawInput(float elapsedFrameTime, float elapsedTotalTime) override;

private:

	static Interval clampInterval;

	float prevTotalTime;
	sf::Vector2i prevPos;
};