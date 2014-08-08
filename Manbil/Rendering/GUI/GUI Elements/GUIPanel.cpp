#include "GUIPanel.h"


void GUIPanel::ScaleBy(Vector2f scaleAmount)
{
    //Scale the 'extents' vector.
    extents = Vector2f(extents.x, extents.y).ComponentProduct(scaleAmount).CastToInt();

    //Scale each element's position to move it relative to this panel's center.
    for (unsigned int i = 0; i < Elements.size(); ++i)
    {
        Vector2i elPos = Elements[i]->GetCollisionCenter();
        Elements[i]->SetPosition(Vector2f(elPos.x, elPos.y).ComponentProduct(scaleAmount).CastToInt());
    }
}

void GUIPanel::Update(float elapsed)
{
    Vector2i nPos = -pos;

    for (unsigned int i = 0; i < Elements.size(); ++i)
    {
        Elements[i]->MoveElement(pos);
        Elements[i]->Update(elapsed);
        Elements[i]->MoveElement(nPos);
    }
}
std::string GUIPanel::Render(float elapsedTime, const RenderInfo & info)
{
    Vector2i nPos = -pos;
    std::string err;
    for (unsigned int i = 0; i < Elements.size(); ++i)
    {
        Elements[i]->MoveElement(pos);
        err = Elements[i]->Render(elapsedTime, info);
        Elements[i]->MoveElement(nPos);

        if (!err.empty()) return err;
    }
    return "";
}

void GUIPanel::OnMouseClick(Vector2i mouseP)
{
    for (unsigned int i = 0; i < Elements.size(); ++i)
        Elements[i]->OnMouseClick(mouseP - Elements[i]->GetCollisionCenter());
}
void GUIPanel::OnMouseDrag(Vector2i oldP, Vector2i currentP)
{
    for (unsigned int i = 0; i < Elements.size(); ++i)
    {
        Vector2i center = Elements[i]->GetCollisionCenter();
        Elements[i]->OnMouseDrag(oldP - center, currentP - center);
    }
}
void GUIPanel::OnMouseRelease(Vector2i mouseP)
{
    for (unsigned int i = 0; i < Elements.size(); ++i)
        Elements[i]->OnMouseRelease(mouseP - Elements[i]->GetCollisionCenter());
}