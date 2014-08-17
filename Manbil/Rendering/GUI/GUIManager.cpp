#include "GUIManager.h"


GUIManager::GUIManager(void) : wasMouseClicked(false), panel(1.0f) { }

void GUIManager::Update(float elapsed, Vector2i mousePos, bool clicked)
{
    Vector2i center = panel.GetCollisionCenter();
    Vector2i relMouse = mousePos - center,
             oldRelMouse = originalClickPos - center;

    panel.Update(elapsed, relMouse);

    //Handle mouse input.
    if (clicked)
    {
        if (wasMouseClicked)
        {
            panel.OnMouseDrag(oldRelMouse, relMouse);
        }
        else
        {
            originalClickPos = mousePos;
            panel.OnMouseClick(relMouse);
        }
    }
    else
    {
        if (wasMouseClicked)
        {
            panel.OnMouseRelease(relMouse);
        }
    }

    wasMouseClicked = clicked;
}

std::string GUIManager::Render(float elapsed, const RenderInfo & info)
{
    return panel.Render(elapsed, info);
}