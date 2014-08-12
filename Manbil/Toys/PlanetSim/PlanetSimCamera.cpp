#include "PlanetSimCamera.h"


bool PlanetSimCamera::Update(float elapsedSeconds)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
        AddRoll(RollSpeed * elapsedSeconds);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
        AddRoll(-RollSpeed * elapsedSeconds);

    return MovingCamera::Update(elapsedSeconds);
}