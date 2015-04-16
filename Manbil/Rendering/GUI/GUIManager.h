#pragma once

#include "GUIElement.h"


//Handles updating, rendering, and mouse input for some "root" GUIElement (usually a panel).
class GUIManager
{
public:


    GUIElementPtr RootElement;

    //Creates a manager with the given root GUIElement.
    GUIManager(GUIElementPtr root = GUIElementPtr(0)) : RootElement(root) { }

    //Updates the root element and sends it the appropriate mouse input message.
    void Update(float elapsed, Vector2i mousePos, bool clicked);
    //Renders the root element.
    void Render(float elapsed, const RenderInfo& info);


private:

    bool wasMouseClicked = true;
    Vector2f originalClickPos;
};