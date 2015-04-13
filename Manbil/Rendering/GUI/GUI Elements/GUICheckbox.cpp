#include "GUICheckbox.h"


GUICheckbox::GUICheckbox(const GUITexture& box, const GUITexture& check,
                         bool hideBoxIfChecked, float timeLerpSpeed)
    : Box(box), Check(check), HideBoxIfChecked(hideBoxIfChecked),
      GUIElement(UniformDictionary(), timeLerpSpeed)
{
    Box.Depth = 0.0f;
    Check.Depth = 0.01f;
}

void GUICheckbox::ToggleCheck(bool raiseEvent)
{
    isChecked = !isChecked;
    if (raiseEvent)
    {
        RaiseOnClickedEvent();
    }
}
void GUICheckbox::SetChecked(bool newVal, bool raiseEvent)
{
    isChecked = newVal;
    if (raiseEvent)
    {
        RaiseOnClickedEvent();
    }
}

Box2D GUICheckbox::GetBounds(void) const
{
    Box2D bx = Box.GetBounds();
    bx.Move(Box.GetPos());
    return bx;
}
bool GUICheckbox::GetDidBoundsChangeDeep(void) const
{
    return DidBoundsChange || Box.DidBoundsChange;
}
void GUICheckbox::ClearDidBoundsChangeDeep(void)
{
    DidBoundsChange = false;
    Box.DidBoundsChange = false;
}
void GUICheckbox::ScaleBy(Vector2f scaleAmount)
{
    SetScale(GetScale().ComponentProduct(scaleAmount));
}
void GUICheckbox::SetScale(Vector2f newScale)
{
    Vector2f oldScale = GetScale();
    GUIElement::SetScale(newScale);

    Vector2f delta(newScale.x / oldScale.x, newScale.y / oldScale.y);
    Box.ScaleBy(delta);
    Check.ScaleBy(delta);
}

void GUICheckbox::Render(float elapsedTime, const RenderInfo & info)
{
    if (isChecked)
    {
        if (HideBoxIfChecked)
        {
            RenderChild(&Check, elapsedTime, info);
        }
        else
        {
            RenderChild(&Box, elapsedTime, info);
            RenderChild(&Check, elapsedTime, info);
        }
    }
    else
    {
        RenderChild(&Box, elapsedTime, info);
    }
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
            Check.Update(elapsed, relativeMousePos);
        }
        else
        {
            Box.Update(elapsed, relativeMousePos);
            Check.Update(elapsed, relativeMousePos);
        }
    }
    else
    {
        Box.Update(elapsed, relativeMousePos);
    }
}

void GUICheckbox::RaiseOnClickedEvent(void)
{
    if (OnClicked != 0)
    {
        OnClicked(this, OnClicked_Data);
    }
}