#include "SFMLWorld.h"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

typedef sf::Clock Clock;

void SFMLWorld::RunWorld(void)
{
	//sfguiWindow = sfg::Window::Create();
	//sfguiDesktop.Add(sfguiWindow);
	

	window = new sf::RenderWindow(sf::VideoMode(windowWidth, windowHeight), "World window", sf::Style::Default, contextSettings);
	Clock cl = Clock();

	totalElapsedSeconds = 0.0f;
	worldOver = false;
	float elapsed;
	sf::Event windowEvent;

	InitializeWorld();

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
			else
			{
				OnOtherWindowEvent(windowEvent);
			}

			//sfgui.HandleEvent(windowEvent);
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
			//sfguiDesktop.Update(elapsed);
			UpdateWorld(elapsed);

			RenderWorld(elapsed);
		}
	}

	OnWorldEnd();
}