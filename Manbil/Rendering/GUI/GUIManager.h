#pragma once

#include "GUIElement.h"


//Handles updating, rendering, and mouse input for some "root" GUIElement (usually a panel).
class GUIManager
{
public:

    //Use either "RootElement" or "RootElementRaw".

    GUIElementPtr RootElement;
    GUIElement* RootElementRaw = 0;


    //Creates a manager with the given root GUIElement.
    GUIManager(GUIElementPtr root = GUIElementPtr(0)) : RootElement(root) { }
    GUIManager(GUIElement* root) : RootElementRaw(root) { }

    //Updates the root element and sends it the appropriate mouse input message.
    void Update(float elapsed, Vector2i mousePos, bool clicked);
    //Renders the root element.
    void Render(float elapsed, const RenderInfo& info);

    GUIElement* GetRoot(void) const { return (RootElementRaw == 0 ? RootElement.get() : RootElementRaw); }
    void SetRoot(GUIElement* rawRoot) { RootElement.reset(); RootElementRaw = rawRoot; }
    void SetRoot(GUIElementPtr root) { RootElementRaw = 0; RootElement = root; }


private:

    bool wasMouseClicked = true;
    Vector2f originalClickPos;
};