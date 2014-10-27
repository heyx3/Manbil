#include "GUICheckbox.h"


Box2D GUICheckbox::GetBounds(void) const
{
    return Box.GetBounds();
}
void GUICheckbox::SetScale(Vector2f newScale)
{
    Vector2f oldScale = GetScale();
    GUIElement::SetScale(newScale);

    Vector2f delta(newScale.x / oldScale.x, newScale.y / oldScale.y);
    Box.ScaleBy(delta);
    Check.ScaleBy(delta);
}

std::string GUICheckbox::Render(float elapsedTime, const RenderInfo & info)
{
    if (isChecked)
    {
        if (HideBoxIfChecked)
        {
            std::string err = RenderChild(&Check, elapsedTime, info);
            if (!err.empty()) return "Error rendering check: " + err;
        }
        else
        {
            std::string err = RenderChild(&Box, elapsedTime, info);
            if (!err.empty()) return "Error rendering box: " + err;

            std::string err = RenderChild(&Check, elapsedTime, info);
            if (!err.empty()) return "Error rendering check: " + err;
        }
    }
    else
    {
        std::string err = RenderChild(&Box, elapsedTime, info);
        if (!err.empty()) return "Error rendering box: " + err;
    }

    return "";
}

void GUICheckbox::OnMouseClick(Vector2f relativeMouse)
{
    if (GetBounds().IsPointInside(relativeMouse))
    {
        ToggleCheck(true);
    }

    if (isChecked)
    {
        if (HideBoxIfChecked)
        {
            Check.OnMouseClick(relativeMouse);
        }
        else
        {
            Box.OnMouseClick(relativeMouse);
            Check.OnMouseClick(relativeMouse);
        }
    }
    else
    {
        Box.OnMouseClick(relativeMouse);
    }
}
void GUICheckbox::OnMouseRelease(Vector2f relativeMouse)
{
    if (isChecked)
    {
        if (HideBoxIfChecked)
        {
            Check.OnMouseRelease(relativeMouse);
        }
        else
        {
            Box.OnMouseRelease(relativeMouse);
            Check.OnMouseRelease(relativeMouse);
        }
    }
    else
    {
        Box.OnMouseRelease(relativeMouse);
    }
}
void GUICheckbox::OnMouseDrag(Vector2f oldRelativeMouse, Vector2f newRelativeMouse)
{
    if (isChecked)
    {
        if (HideBoxIfChecked)
        {
            Check.OnMouseDrag(oldRelativeMouse, newRelativeMouse);
        }
        else
        {
            Box.OnMouseDrag(oldRelativeMouse, newRelativeMouse);
            Check.OnMouseDrag(oldRelativeMouse, newRelativeMouse);
        }
    }
    else
    {
        Box.OnMouseDrag(oldRelativeMouse, newRelativeMouse);
    }
}

void GUICheckbox::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    if (isChecked)
    {
        if (HideBoxIfChecked)
        {
            if (Check.GetDidBoundsChange())
                DidBoundsChange = true;

            Check.Update(elapsed, relativeMousePos);
        }
        else
        {
            if (Check.GetDidBoundsChange() || Box.GetDidBoundsChange())
                DidBoundsChange = true;

            Box.Update(elapsed, relativeMousePos);
            Check.Update(elapsed, relativeMousePos);
        }
    }
    else
    {
        if (Box.GetDidBoundsChange())
            DidBoundsChange = true;

        Box.Update(elapsed, relativeMousePos);
    }
}