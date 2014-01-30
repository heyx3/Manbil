#pragma once

#include "Deadzones.h"


//Represents an input that can range from { -1, -1 } to { 1, 1 }.
//Supports several different types of deadzones.
class Vector2Input
{
public:

	DeadzonePtr DeadzoneFilter;


	Vector2f GetValue(void) const { return value; }
	Vector2f GetPreviousValue(void) const { return previousValue; }

	//Gets the change in this input's value from the last Update() call to the current one.
	Vector2f GetValueDelta(void) const { return value - previousValue; }


	Vector2Input(DeadzonePtr filter, Vector2f startingValue = Vector2f())
		: value(startingValue), previousValue(startingValue), DeadzoneFilter(filter)
	{

	}
	virtual ~Vector2Input(void) { }

	void Update(float elapsedFrameTime, float elapsedTotalTime)
	{
		previousValue = value;
		value = DeadzoneFilter->Filter(GetRawInput(elapsedFrameTime, elapsedTotalTime));
	}


protected:

	virtual Vector2f GetRawInput(float elapsedFrameTime, float elapsedTotalTime) = 0;


private:

	Vector2f value, previousValue;
};