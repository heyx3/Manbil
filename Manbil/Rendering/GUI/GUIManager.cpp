#include "GUIManager.h"



void GUIManager::Update(float elapsed, Vector2i mousePos, bool clicked)
{
    Vector2f center = panel.GetCollisionCenter();
    Vector2f relMouse = ToV2f(mousePos) - center,
             oldRelMouse = originalClickPos - center;

    GetRoot()->Update(elapsed, relMouse);

    //Handle mouse input.
    if (clicked)
    {
        if (wasMouseClicked)
        {
            GetRoot()->OnMouseDrag(oldRelMouse, relMouse);
        }
        else
        {
            originalClickPos = ToV2f(mousePos);
            GetRoot()->OnMouseClick(relMouse);
        }
    }
    else
    {
        if (wasMouseClicked)
        {
            GetRoot()->OnMouseRelease(relMouse);
        }
    }

    wasMouseClicked = clicked;
}

std::string GUIManager::Render(float elapsed, const RenderInfo & info)
{
    return GetRoot()->Render(elapsed, info);
}