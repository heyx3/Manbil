#pragma once

//#include <SFGUI/SFGUI.hpp>
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

//A basic world using SFML. Also initializes the SFGUI library upon creation.
class SFMLWorld
{
public:

	SFMLWorld(int windWidth, int windHeight)
		: totalElapsedSeconds(0.0f), window(0), windowWidth(windWidth), windowHeight(windHeight) { window = 0; }
	virtual ~SFMLWorld(void) { if (window) delete window; }

	float GetTotalElapsedSeconds(void) const { return  totalElapsedSeconds; }
	sf::RenderWindow * GetWindow(void) const { return window; }

	void SetWindowTitle(const sf::String & title) { window->setTitle(title); }

	bool IsGameOver(void) const { return worldOver; }
	
	void EndWorld(void) { worldOver = true; }

	//Starts running this world in an endless loop until it's finished.
	void RunWorld(void);

	//Gets the object that manages the SFGUI system.
	//sfg::SFGUI & GetSFGUI(void) { return sfgui; }
	//Gets the object that manages SFGUI windows.
	//sfg::Desktop & GetGUIDesktop(void) { return sfguiDesktop; }
	//Gets the object that manages a set of GUI elements.
	//sfg::Window::Ptr & GetGUIWindow(void) { return sfguiWindow; }

	//Draws SFGUI elements onto this world's window.
	//void DrawGUI(void) { sfgui.Display(*window); }

protected:

	template<class SomeClass>
	void DeleteAndSetToNull(SomeClass *& pointer) { if (pointer) delete pointer; pointer = 0; }

	virtual void InitializeWorld(void) { }

	virtual void UpdateWorld(float elapsedSeconds) = 0;
	virtual void RenderWorld(float elapsedSeconds) = 0;

	virtual void OnCloseWindow(void) { worldOver = true; }
	virtual void OnWindowResized(unsigned int newWidth, unsigned int newHeight) { }
	virtual void OnOtherWindowEvent(sf::Event & windowEvent) { }

	virtual void OnWorldEnd(void) { }

private:

	//sfg::SFGUI sfgui;
	//sfg::Desktop sfguiDesktop;
	//sfg::Window::Ptr sfguiWindow;

	float totalElapsedSeconds;
	sf::RenderWindow * window;
	int windowWidth, windowHeight;

	bool worldOver;
};