#pragma once

#include "../Vector2Input.h"
#include "../../Math/Interval.h"
#include <SFML/Window/Mouse.hpp>

//Gets the delta position of the mouse per second.
//Optionally resets the mouse to a given position every time this input is updated.
//NOT GUARANTEED TO STAY INSIDE THE RANGE {-1, 1}.
class MouseDeltaVector2Input : Vector2Input
{
public:

    bool UseHorizontal, UseVertical;

    //Scales the mouse movement.
    Vector2f SpeedScale;
	//If X or Y is set to -1, this field is not used.
	//Otherwise, sets the mouse to this position every time GetRawInput() is called.
	sf::Vector2i MouseResetPos;


    MouseDeltaVector2Input(Vector2f speedScale, DeadzonePtr deadzone,
						   sf::Vector2i mouseResetPos = sf::Vector2i(-1, -1), Vector2f initialPos = Vector2f())
		: Vector2Input(deadzone, initialPos), SpeedScale(speedScale), MouseResetPos(mouseResetPos), prevPos(mouseResetPos), prevTotalTime(-1.0f),
          UseHorizontal(true), UseVertical(true)
	{

	}

protected:

	virtual Vector2f GetRawInput(float elapsedFrameTime, float elapsedTotalTime) override;

private:

	float prevTotalTime;
	sf::Vector2i prevPos;
};