#pragma once

#include "../BoolInput.h"
#include "../FloatInput.h"
#include "../Vector2Input.h"

typedef std::shared_ptr<BoolInput> BoolInputPtr;
typedef std::shared_ptr<FloatInput> FloatInputPtr;

//Takes "up", "down", "left", and "right" buttons and combines them into a Vector2f
//    in the range { -1, -1 }, { 1, 1 }.
class FourButtonVector2Input : public Vector2Input
{
public:

	BoolInputPtr PositiveX, NegativeX,
				 PositiveY, NegativeY;
	FourButtonVector2Input(BoolInputPtr posX, BoolInputPtr negX, BoolInputPtr posY, BoolInputPtr negY)
		: Vector2Input(DeadzonePtr((Deadzone*)(new EmptyDeadzone()))),
		  PositiveX(posX), NegativeX(negX), PositiveY(posY), NegativeY(negY)
	{

	}

protected:

	virtual Vector2f GetRawInput(float elapsedFrameTime, float elapsedTotalTime) override;
};


//Takes "X" and "Y" float inputs and combines them into a Vector2f in the range { -1, -1 }, { 1, 1 }.
class TwoAxisVector2Input : public Vector2Input
{
public:

	FloatInputPtr XAxis, YAxis;
	TwoAxisVector2Input(DeadzonePtr deadzone, FloatInputPtr xAxis, FloatInputPtr yAxis, Vector2f startingValue = Vector2f())
		: Vector2Input(deadzone, startingValue), XAxis(xAxis), YAxis(yAxis)
	{

	}

protected:

	virtual Vector2f GetRawInput(float elapsed, float total) override
	{
		XAxis->Update(elapsed, total);
		YAxis->Update(elapsed, total);
		//Don't forget to map from [0, 1] to [-1, 1].
		return Vector2f(-1.0f + (2.0f * XAxis->GetValue()),
					    -1.0f + (2.0f * YAxis->GetValue()));
	}
};