#include "GUICheckbox.h"


Vector2f GUICheckbox::GetCollisionDimensions(void) const
{
    if (isChecked)
    {
        if (HideBoxIfChecked)
        {
            return Check.GetCollisionDimensions();
        }
        else
        {
            Vector2f box = Box.GetCollisionDimensions(),
                     check = Check.GetCollisionDimensions();
            return Vector2f(BasicMath::Max(box.x, check.x),
                            BasicMath::Max(box.y, check.y));
        }
    }
    else
    {
        return Box.GetCollisionDimensions();
    }
}
void GUICheckbox::SetScale(Vector2f newScale)
{
    Vector2f delta(newScale.x / scale.x, newScale.y / scale.y);
    scale = newScale;
    Box.ScaleBy(delta);
    Check.ScaleBy(delta);
}

std::string GUICheckbox::Render(float elapsedTime, const RenderInfo & info)
{
    Vector4f myCol = GetColor();

    if (isChecked)
    {
        if (HideBoxIfChecked)
        {
            std::string err = RenderCheck(elapsedTime, Depth, myCol, info);
            if (!err.empty()) return "Error rendering check: " + err;
        }
        else
        {
            std::string err = RenderBox(elapsedTime, myCol, info);
            if (!err.empty()) return "Error rendering box: " + err;

            err = RenderCheck(elapsedTime, Depth + 0.00001f, myCol, info);
            if (!err.empty()) return "Error rendering check: " + err;
        }
    }
    else
    {
        std::string err = RenderBox(elapsedTime, myCol, info);
        if (!err.empty()) return "Error rendering box: " + err;
    }

    return "";
}
std::string GUICheckbox::RenderBox(float elapsed, Vector4f myCol, const RenderInfo & info)
{
    Box.SetPosition(Center);
    Box.Depth = Depth;
    Vector4f oldCol = Box.GetColor();
    Box.SetColor(oldCol.ComponentProduct(myCol));

    std::string err = Box.Render(elapsed, info);

    Box.SetColor(oldCol);

    return err;
}
std::string GUICheckbox::RenderCheck(float elapsed, float depth, Vector4f myCol, const RenderInfo & info)
{
    Check.SetPosition(Center);
    Check.Depth = depth;
    Vector4f oldCol = Check.GetColor();
    Check.SetColor(oldCol.ComponentProduct(myCol));

    std::string err = Check.Render(elapsed, info);

    Check.SetColor(oldCol);

    return err;
}

void GUICheckbox::OnMouseClick(Vector2f relativeMouse)
{
    if (IsLocalInsideBounds(relativeMouse))
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