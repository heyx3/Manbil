#include "GUIManager.h"



void GUIManager::Update(float elapsed, Vector2i mousePos, bool clicked)
{
    GUIElement* root = GetRoot();

    Vector2f relMouse = ToV2f(mousePos) - root->GetPos(),
             oldRelMouse = originalClickPos - root->GetPos();

    root->Update(elapsed, relMouse);

    //Handle mouse input.
    if (clicked)
    {
        if (wasMouseClicked)
        {
            root->OnMouseDrag(oldRelMouse, relMouse);
        }
        else
        {
            originalClickPos = ToV2f(mousePos);
            root->OnMouseClick(relMouse);
        }
    }
    else
    {
        if (wasMouseClicked)
        {
            root->OnMouseRelease(relMouse);
        }
    }

    root->DidBoundsChange = false;
    wasMouseClicked = clicked;
}

void GUIManager::Render(float elapsed, const RenderInfo& info)
{
    DrawingQuad::GetInstance()->GetMesh().Transform = TransformObject();
    GetRoot()->Render(elapsed, info);
}