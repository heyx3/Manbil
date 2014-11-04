#include "GUIPanel.h"



Box2D GUIPanel::GetBounds(void) const
{
    Box2D bounds;

    if (elements.empty())
        bounds = Box2D();
    bounds = elements[0]->GetBounds();
    for (unsigned int i = 1; i < elements.size(); ++i)
        bounds = Box2D(bounds, elements[i]->GetBounds());

    return bounds;
}
bool GUIPanel::GetDidBoundsChangeDeep(void) const
{
    if (DidBoundsChange) return true;

    for (unsigned int i = 0; i < elements.size(); ++i)
        if (elements[i]->GetDidBoundsChangeDeep())
            return true;

    return false;
}

void GUIPanel::ScaleBy(Vector2f scaleAmount)
{
    GUIElement::ScaleBy(scaleAmount);

    //Scale each element's position to move it relative to this panel's center.
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        bool oldDidChange = elements[i]->DidBoundsChange;

        elements[i]->SetPosition(elements[i]->GetPos().ComponentProduct(scaleAmount));
        elements[i]->ScaleBy(scaleAmount);

        elements[i]->DidBoundsChange = oldDidChange;
    }
}
void GUIPanel::SetScale(Vector2f newScale)
{
    Vector2f oldScale = GetScale();
    Vector2f delta(newScale.x / oldScale.x, newScale.y / oldScale.y);
    ScaleBy(delta);
}

void GUIPanel::AddElement(GUIElementPtr element)
{
    if (std::find(elements.begin(), elements.end(), element) == elements.end())
    {
        elements.insert(elements.end(), element);
    }
}
bool GUIPanel::RemoveElement(GUIElementPtr element)
{
    auto loc = std::find(elements.begin(), elements.end(), element);
    if (loc == elements.end())
        return false;

    elements.erase(loc);

    return true;
}
bool GUIPanel::ContainsElement(GUIElementPtr element) const
{
    return (std::find(elements.begin(), elements.end(), element) != elements.end());
}

void GUIPanel::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        Vector2f relPos = relativeMousePos - elements[i]->GetPos();
        elements[i]->Update(elapsed, relPos);
    }
}
std::string GUIPanel::Render(float elapsedTime, const RenderInfo & info)
{
    //Instead of returning an error as soon as it is found,
    //    render all sub-elements and collect any errors into one big error string.
    std::string err = "";
    unsigned int line = 0;

    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        std::string tempErr = RenderChild(elements[i].get(), elapsedTime, info);
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
        elements[i]->OnMouseClick(mouseP - elements[i]->GetPos());
}
void GUIPanel::OnMouseDrag(Vector2f oldP, Vector2f currentP)
{
    for (unsigned int i = 0; i < elements.size(); ++i)
    {
        Vector2f center = elements[i]->GetPos();
        elements[i]->OnMouseDrag(oldP - center, currentP - center);
    }
}
void GUIPanel::OnMouseRelease(Vector2f mouseP)
{
    for (unsigned int i = 0; i < elements.size(); ++i)
        elements[i]->OnMouseRelease(mouseP - elements[i]->GetPos());
}