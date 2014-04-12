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

    //Keep it inside the allowable range.
    delta.x = BasicMath::Clamp(delta.x, -MaxSpeed.x * elapsedFrameTime, MaxSpeed.x * elapsedFrameTime);
    delta.y = BasicMath::Clamp(delta.y, -MaxSpeed.y * elapsedFrameTime, MaxSpeed.y * elapsedFrameTime);
    //Scale the delta to be between -1 and 1.
    delta = Vector2f(delta.x / (MaxSpeed.x * elapsedFrameTime), delta.y / (MaxSpeed.y * elapsedFrameTime));


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

    Vector2f signs(BasicMath::Sign(delta.x), BasicMath::Sign(delta.y));
    return Vector2f(signs.x * powf(signs.x * delta.x, 1.0f), signs.y * powf(signs.y * delta.y, 1.0f));
	return delta;
}