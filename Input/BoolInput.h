#pragma once

#include <assert.h>


//Represnts an input that can be true or false.
class BoolInput
{
public:

	//Several different states may be desired depending on the type of input.
	enum ValueStates
	{
		IsDown,
		IsNotDown,

		JustPressed,
		JustReleased,
	};
	ValueStates DesiredValueState;


	//Get input values.

	bool GetValue(void) const { return value; }
	bool GetPreviousValue(void) const { return previousValue; }


	//Constructor/deconstructor.

	BoolInput(bool startingValue = false, ValueStates desiredValueState = ValueStates::IsDown)
		: value(startingValue), DesiredValueState(desiredValueState) { }
	virtual ~BoolInput(void) { }


	//Update this input's current input value.
	void Update(float elapsedFrameTime, float elapsedTotalTime)
    {
        bool val = GetRawInput(elapsedFrameTime, elapsedTotalTime);
		previousValue = value;
        if (val)
            value = value;

		switch (DesiredValueState)
		{
			case ValueStates::IsDown:
				value = val;
				break;
			case ValueStates::IsNotDown:
				value = !val;
				break;
			case ValueStates::JustPressed:
				value = (val && !previousValue);
				break;
			case ValueStates::JustReleased:
				value = (!val && previousValue);
			default: assert(false); return;
		}
	}


protected:

	//Get the raw value of the current input (yet to be filtered according to DesiredValueState).
	virtual bool GetRawInput(float elapsedFrameTime, float elapsedTotalTime) = 0;


private:
	
	bool value, previousValue;
};