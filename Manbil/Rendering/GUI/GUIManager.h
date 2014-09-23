#pragma once

#include "GUI Elements/GUIPanel.h"
#include "GUI Elements/GUIFormattedPanel.h"


//Handles updating, rendering, and mouse input for some "root" GUIElement (usually a panel).
class GUIManager
{
public:


    GUIElementPtr RootElement;

    //Creates a manager with the given root GUIElement.
    GUIManager(GUIElementPtr root = GUIElementPtr(0)) : RootElement(root) { }

    //Updates the root element and sends it the appropriate mouse input message.
    void Update(float elapsed, Vector2i mousePos, bool clicked);
    //Returns an error message, or the empty string if everything went fine.
    std::string Render(float elapsed, const RenderInfo & info);


private:

    bool wasMouseClicked;
    Vector2f originalClickPos;
};