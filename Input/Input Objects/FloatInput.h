#pragma once

#include "../../Interval.h"
#include <assert.h>

//Represents an input that can span the range 0 to 1.
//Will either clamp the raw input to the range [0, 1],
//    or map it from some "source" range to the range [0, 1].
class FloatInput
{
public:

	//Creates a float input that clamps the raw value to the range [0, 1].
	FloatInput(float startingValue = 0.0f) : mode(Modes::Clamp), source(0.0f, 1.0f, 0.001f), value(startingValue), previousValue(startingValue) { }
	//Creates a float input that maps the raw value from the given range to the range [0, 1].
	FloatInput(Interval sourceRange, float startingValue = 0.0f) : mode(Modes::Map), source(sourceRange), value(startingValue), previousValue(startingValue) { }


	float GetValue(void) const { return value; }
	float GetPreviousValue(void) const { return previousValue; }

	void Update(float elapsedFrameTime, float elapsedTotalTime)
	{
		float val = GetRawValue(elapsedFrameTime, elapsedTotalTime);
		previousValue = value;

		switch (mode)
		{
			case Modes::Clamp:
				value = source.ClampValueToInterval(val);
				break;
			case Modes::Map:
				value = source.MapValue(Interval(0.0f, 1.0f, 0.001f), value);
				break;
			default: assert(false); break;
		}
	}

protected:

	virtual float GetRawValue(float elapsedFrameTime, float elapsedTotalTime) = 0;

private:

	float value, previousValue;

	enum Modes
	{
		//Clamp the input to the range [0, 1].
		Clamp,
		//Map the input from the "source" range to [0, 1].
		Map,
	};
	Modes mode;
	
	Interval source;
};