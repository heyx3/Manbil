#pragma once

#include "GUI Elements/GUIPanel.h"


//Manages GUIElement input and updating/rendering.
class GUIManager
{
public:


    const GUIPanel & GetRoot(void) const { return panel; }
    GUIPanel & GetRoot(void) { return panel; }

    GUIManager(void);

    void Update(float elapsed, Vector2i mousePos, bool clicked);

    //Returns an error message, or the empty string if everything went fine.
    std::string Render(float elapsed, const RenderInfo & info);


private:

    GUIPanel panel;

    bool wasMouseClicked;
    Vector2f originalClickPos;
};