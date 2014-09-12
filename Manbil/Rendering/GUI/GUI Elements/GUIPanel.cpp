#include "GUIPanel.h"



void GUIPanel::AddElement(GUIElement* element)
{
    if (std::find(elements.begin(), elements.end(), element) == elements.end())
        elements.insert(elements.end(), element);
}
bool GUIPanel::RemoveElement(GUIElement* element)
{
    auto loc = std::find(elements.begin(), elements.end(), element);
    if (loc == elements.end())
        return false;

    elements.erase(loc);
    return true;
}
bool GUIPanel::ContainsElement(GUIElement* element) const
{
    return (std::find(elements.begin(), elements.end(), element) != elements.end());
}

void GUIPanel::ScaleBy(Vector2f scaleAmount)
{
    //Scale the 'extents' vector.
    extents = Vector2f(extents.x, extents.y).ComponentProduct(scaleAmount);

    //Scale each element's position to move it relative to this panel's center.
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        Vector2f elPos = elements[i]->GetCollisionCenter();
        elements[i]->SetPosition(Vector2f(elPos.x, elPos.y).ComponentProduct(scaleAmount));
    }
}
void GUIPanel::SetScale(Vector2f newScale)
{
    Vector2f dims = GetCollisionDimensions();
    Vector2f delta(newScale.x / dims.x, newScale.y / dims.y);
    ScaleBy(delta);
}

void GUIPanel::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        Vector2f relPos = relativeMousePos - elements[i]->GetCollisionCenter();
        elements[i]->Update(elapsed, relPos);
    }
}
std::string GUIPanel::Render(float elapsedTime, const RenderInfo & info)
{
    Vector2f nPos = -pos;

    //Instead of returning an error as soon as it is found,
    //    render all sub-elements and collect any errors into one big error string.
    std::string err = "";
    unsigned int line = 0;

    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        elements[i]->MoveElement(pos);
        elements[i]->Depth += Depth;
        std::string tempErr = elements[i]->Render(elapsedTime, info);
        elements[i]->MoveElement(nPos);
        elements[i]->Depth -= Depth;

        if (!tempErr.empty())
        {
            if (line > 0) err += "\n";
            line += 1;

            err += std::to_string(line) + ") " + tempErr;
        }
    }

    return err;
}

void GUIPanel::OnMouseClick(Vector2f mouseP)
{
    for (unsigned int i = 0; i < elements.size(); ++i)
        elements[i]->OnMouseClick(mouseP - elements[i]->GetCollisionCenter());
}
void GUIPanel::OnMouseDrag(Vector2f oldP, Vector2f currentP)
{
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        Vector2f center = elements[i]->GetCollisionCenter();
        elements[i]->OnMouseDrag(oldP - center, currentP - center);
    }
}
void GUIPanel::OnMouseRelease(Vector2f mouseP)
{
    for (unsigned int i = 0; i < elements.size(); ++i)
        elements[i]->OnMouseRelease(mouseP - elements[i]->GetCollisionCenter());
}