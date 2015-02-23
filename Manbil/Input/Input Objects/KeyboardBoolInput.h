#pragma once

#include <SFML/Window/Keyboard.hpp>
#include "../BoolInput.h"

//A BoolInput that uses a keyboard button.
class KeyboardBoolInput : BoolInput
{
public:

	typedef sf::Keyboard::Key Key;
	Key KeyToCheck;

	KeyboardBoolInput(Key toCheck, ValueStates desiredVS = ValueStates::IsDown) : BoolInput(desiredVS != ValueStates::IsNotDown, desiredVS), KeyToCheck(toCheck) { }

protected:
    
#pragma warning(disable: 4100)
	virtual bool GetRawInput(float el, float total) override { return sf::Keyboard::isKeyPressed(KeyToCheck); }
#pragma warning(default: 4100)
};