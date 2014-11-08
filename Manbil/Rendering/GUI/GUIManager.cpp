#include "GUIManager.h"



void GUIManager::Update(float elapsed, Vector2i mousePos, bool clicked)
{
    Box2D bounds = RootElement->GetBounds();
    bounds.Move(RootElement->GetPos());
    Vector2f relMouse = ToV2f(mousePos) - bounds.GetCenter(),
             oldRelMouse = originalClickPos - bounds.GetCenter();

    RootElement->Update(elapsed, relMouse);

    //Handle mouse input.
    if (clicked)
    {
        if (wasMouseClicked)
        {
            RootElement->OnMouseDrag(oldRelMouse, relMouse);
        }
        else
        {
            originalClickPos = ToV2f(mousePos);
            RootElement->OnMouseClick(relMouse);
        }
    }
    else
    {
        if (wasMouseClicked)
        {
            RootElement->OnMouseRelease(relMouse);
        }
    }

    wasMouseClicked = clicked;
}

std::string GUIManager::Render(float elapsed, const RenderInfo & info)
{
    return RootElement->Render(elapsed, info);
}