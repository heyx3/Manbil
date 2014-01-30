#include "MouseDeltaVector2Input.h"

Interval MouseDeltaVector2Input::clampInterval = Interval(-1.0f, 1.0f, 0.001f, true);


Vector2f MouseDeltaVector2Input::GetRawInput(float elapsedFrameTime, float elapsedTotalTime)
{
	//Update timing.
	float deltaTime = elapsedTotalTime - prevTotalTime;
	prevTotalTime = elapsedTotalTime;

	//Calculate delta.
	sf::Vector2i posI = sf::Mouse::getPosition();
	Vector2f delta = Vector2f(posI.x - prevPos.x, posI.y - prevPos.y);

	//Update "prevPos". If necessary, reset the mouse position first.
	if (MouseResetPos.x != -1 && MouseResetPos.y != -1)
	{
		sf::Mouse::setPosition(MouseResetPos);
		prevPos = MouseResetPos;
	}
	else
	{
		prevPos = posI;
	}

	//Return value delta per second.
	Vector2f finalVal = delta / (MaxSpeed * deltaTime);
	return Vector2f(clampInterval.ClampValueToInterval(finalVal.x), clampInterval.ClampValueToInterval(finalVal.y));
}