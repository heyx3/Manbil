#include "MovingCamera.h"

bool MovingCamera::Update(float elapsedTime)
{
	#pragma region WASD EQ

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		IncrementPosition(GetForward() * moveSpeed * elapsedTime);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		IncrementPosition(-GetForward() * moveSpeed * elapsedTime);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		IncrementPosition(GetSideways() * moveSpeed * elapsedTime);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		IncrementPosition(-GetSideways() * moveSpeed * elapsedTime);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
	{
		IncrementPosition(GetUpward() * moveSpeed * elapsedTime);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		IncrementPosition(GetUpward() * -moveSpeed * elapsedTime);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		return true;
	}

	#pragma endregion

	#pragma region Mouse

	if (Window == 0)
	{
		return false;
	}

	Window->setMouseCursorVisible(!capMouse);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		if (!pressedSpace)
		{
			Window->setMouseCursorVisible(true);
			pressedSpace = true;
			capMouse = !capMouse;
		}
	}
	else pressedSpace = false;

	if (!capMouse) return false;

	Vector2i mousePos = Conv(sf::Mouse::getPosition(*Window));
	Vector2i delta = mousePos - mouseTarget;

	Vector2f rotAmount(delta.x * rotSpeed * elapsedTime, delta.y * rotSpeed * elapsedTime);

	sf::Mouse::setPosition(Conv(mouseTarget), *Window);

	AddPitch(rotAmount.y);
	AddYaw(rotAmount.x);

	#pragma endregion

	return false;
}