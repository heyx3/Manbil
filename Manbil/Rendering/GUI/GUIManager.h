#pragma once

#include "GUI Elements/GUIPanel.h"
#include "GUI Elements/GUIFormattedPanel.h"


//Manages GUIElement input and updating/rendering.
//Uses either a normal GUIPanel or a GUIFormattedPanel.
class GUIManager
{
public:

    bool IsFormatted(void) const { return isFormatted; }

    const GUIElement * GetRoot(void) const { return (isFormatted ? (GUIElement*)&formatPanel : (GUIElement*)&panel); }
    GUIElement * GetRoot(void) { return (isFormatted ? (GUIElement*)&formatPanel : (GUIElement*)&panel); }

    const GUIPanel & GetNormalRoot(void) const { return panel; }
    GUIPanel & GetNormalRoot(void) { return panel; }

    const GUIFormattedPanel GetFormattedRoot(void) const { return formatPanel; }
    GUIFormattedPanel & GetFormattedRoot(void) { return formatPanel; }


    //Creates an auto-formatted panel.
    GUIManager(void) : isFormatted(true), panel(UniformDictionary(), Vector2f()), formatPanel(UniformDictionary()) { }
    //Creates a normal panel.
    GUIManager(Vector2f extents) : isFormatted(false), formatPanel(UniformDictionary()), panel(UniformDictionary(), extents) { }


    void Update(float elapsed, Vector2i mousePos, bool clicked);

    //Returns an error message, or the empty string if everything went fine.
    std::string Render(float elapsed, const RenderInfo & info);


private:

    GUIPanel panel;
    GUIFormattedPanel formatPanel;

    bool isFormatted;

    bool wasMouseClicked;
    Vector2f originalClickPos;
};