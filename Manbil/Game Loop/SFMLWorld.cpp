#include "SFMLWorld.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>


typedef sf::Clock Clock;



SFMLWorld::SFMLWorld(int windWidth, int windHeight, sf::ContextSettings settings)
    : contextSettings(settings), totalElapsedSeconds(0.0f), window(0),
      windowWidth(windWidth), windowHeight(windHeight)
{
    window = 0;
}
SFMLWorld::~SFMLWorld(void)
{
    if (window)
    {
        delete window;
    }
}

void SFMLWorld::RunWorld(void)
{
	window = new sf::RenderWindow(GetModeToUse(windowWidth, windowHeight),
                                  GetWindowTitle().c_str(),
                                  GetSFStyleFlags(), contextSettings);
	Clock cl = Clock();

	totalElapsedSeconds = 0.0f;
	worldOver = false;
	float elapsed;
	sf::Event windowEvent;

	InitializeWorld();
    windowHasFocus = true;

	cl.restart();

	while (window->isOpen() && !worldOver)
	{
		//Handle window events first.
		while (window->pollEvent(windowEvent))
		{
			if (windowEvent.type == sf::Event::Closed)
			{
				OnCloseWindow();

			}
			else if (windowEvent.type == sf::Event::Resized)
			{
				OnWindowResized(windowEvent.size.width, windowEvent.size.height);
			}
            else if (windowEvent.type == sf::Event::LostFocus)
            {
                windowHasFocus = false;
                OnWindowLostFocus();
            }
            else if (windowEvent.type == sf::Event::GainedFocus)
            {
                windowHasFocus = true;
                OnWindowGainedFocus();
            }
			else
			{
				OnOtherWindowEvent(windowEvent);
			}
		}

		//Get elapsed time.
		elapsed = cl.getElapsedTime().asSeconds();
		cl.restart();
		totalElapsedSeconds += elapsed;

        //Update various managers.
        Input.Update(elapsed, totalElapsedSeconds);
        Timers.UpdateTimers(elapsed);

		//Update and render.
		if (elapsed > 0.0f)
		{
			UpdateWorld(elapsed);

			RenderWorld(elapsed);
		}
	}

	OnWorldEnd();

    delete window;
    window = 0;
}