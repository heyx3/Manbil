#include "MouseDeltaVector2Input.h"


Vector2f MouseDeltaVector2Input::GetRawInput(float elapsedFrameTime, float elapsedTotalTime)
{
	//Update timing.
	float deltaTime = elapsedTotalTime - prevTotalTime;
	prevTotalTime = elapsedTotalTime;

	//Calculate delta.
	sf::Vector2i posI = sf::Mouse::getPosition();
	Vector2f delta = Vector2f(posI.x - prevPos.x, posI.y - prevPos.y);
    if (!UseHorizontal) delta.x = 0.0f;
    if (!UseVertical) delta.y = 0.0f;
    delta.MultiplyComponents(SpeedScale);

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

    return delta;
}