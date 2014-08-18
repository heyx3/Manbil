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
void GUIPanel::SetScale(Vector2f newScale)
{
    Vector2f dims = ToV2f(GetCollisionDimensions());
    Vector2f delta(newScale.x / dims.x, newScale.y / dims.y);
    ScaleBy(delta);
}

void GUIPanel::CustomUpdate(float elapsed, Vector2i relativeMousePos)
{
    Vector2i nPos = -pos;

    for (unsigned int i = 0; i < Elements.size(); ++i)
    {
        Elements[i]->MoveElement(pos);
        Elements[i]->Update(elapsed, relativeMousePos);
        Elements[i]->MoveElement(nPos);
    }
}
std::string GUIPanel::Render(float elapsedTime, const RenderInfo & info)
{
    Vector2i nPos = -pos;

    //Instead of returning an error as soon as it is found,
    //    render all sub-elements and collect any errors into one big error string.
    std::string err = "";
    unsigned int line = 0;

    for (unsigned int i = 0; i < Elements.size(); ++i)
    {
        Elements[i]->MoveElement(pos);
        std::string tempErr = Elements[i]->Render(elapsedTime, info);
        Elements[i]->MoveElement(nPos);

        if (!tempErr.empty())
        {
            if (line > 0) err += "\n";
            line += 1;

            err += std::to_string(line) + ") " + tempErr;
        }
    }

    return err;
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