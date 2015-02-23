#pragma once

#include "../BoolInput.h"

#include <SFML/Window/Mouse.hpp>


//A bool input based on a mouse button.
class MouseBoolInput : BoolInput
{
public:

	sf::Mouse::Button Button;

	MouseBoolInput(sf::Mouse::Button button, ValueStates state, bool startingValue = false)
		: BoolInput(startingValue, state), Button(button)
	{

	}

protected:
    
#pragma warning(disable: 4100)
	virtual bool GetRawInput(float elapsed, float total) override
	{
		return sf::Mouse::isButtonPressed(Button);
	}
#pragma warning(default: 4100)
};