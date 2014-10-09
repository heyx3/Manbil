#include "GUISlider.h"


Vector2f GUISlider::GetCollisionDimensions(void) const
{
    // The length along the slider axis is the length of the slider plus the length of half the nub on each side.
    // The length perpendicular to the slider axis is the maximum length between the bar and the nub.

    Vector2f barBounds = Bar.GetCollisionDimensions(),
             nubBounds = Nub.GetCollisionDimensions();
    if (IsVertical)
    {
        return Vector2f(BasicMath::Max(barBounds.x, nubBounds.x),
                        barBounds.y + nubBounds.y);
    }
    else
    {
        return Vector2f(barBounds.x + nubBounds.x, BasicMath::Max(barBounds.y, nubBounds.y));
    }
}


void GUISlider::SetScale(Vector2f newScale)
{
    Vector2f oldBarScale = Bar.GetScale();
    Vector2f deltaScale(newScale.x / oldBarScale.x, newScale.y / oldBarScale.y);

    Bar.SetScale(newScale);
    Nub.ScaleBy(deltaScale);
}

float GUISlider::GetNewValue(Vector2f mousePos) const
{
    Vector2f barBounds = Bar.GetCollisionDimensions();

    if (IsVertical)
    {
        return BasicMath::Clamp(BasicMath::LerpComponent(-barBounds.y * 0.5f,
                                                         barBounds.y * 0.5f,
                                                         mousePos.y),
                                0.0f, 1.0f);
    }
    else
    {
        return BasicMath::Clamp(BasicMath::LerpComponent(-barBounds.x * 0.5f,
                                                         barBounds.x * 0.5f,
                                                         mousePos.x),
                                0.0f, 1.0f);
    }
}

std::string GUISlider::Render(float elapsedTime, const RenderInfo & info)
{
    Vector4f myCol = *(Vector4f*)&Params.FloatUniforms[GUIMaterials::QuadDraw_Color].Value;


    //Render the bar.

    Bar.SetPosition(center);
    Bar.Depth = Depth;

    Vector4f oldCol = Bar.GetColor();
    Bar.SetColor(Bar.GetColor().ComponentProduct(myCol));

    std::string err = Bar.Render(elapsedTime, info);
    Bar.SetColor(oldCol);

    if (!err.empty()) return "Error rendering slider bar: " + err;


    //Render the nub.

    Vector2f dims = GetCollisionDimensions();
    Nub.SetPosition(center +
                     (IsVertical ?
                        Vector2f(0.0f, BasicMath::Lerp(-dims.y * 0.5f, dims.y * 0.5f, Value)) :
                        Vector2f(BasicMath::Lerp(-dims.x * 0.5f, dims.x * 0.5f, Value))));
    Nub.Depth = Depth + 0.00001f;

    oldCol = Nub.GetColor();
    Nub.SetColor(oldCol.ComponentProduct(myCol));

    err = Nub.Render(elapsedTime, info);
    Nub.SetColor(oldCol);

    if (!err.empty()) return "Error rendering slider nub: " + err;


    return "";
}

void GUISlider::OnMouseClick(Vector2f mousePos)
{
    if (IsClickable && IsLocalInsideBounds(mousePos))
    {
        Vector2f dims = GetCollisionDimensions();
        Value = GetNewValue(mousePos);
        
        if (UsesTimeLerp())
        {
            CurrentTimeLerpSpeed = TimeLerpSpeed;
            SetTimeLerp(0.0f);
        }

        Nub.OnMouseClick(mousePos - Nub.GetCollisionCenter());
        Bar.OnMouseClick(mousePos - Bar.GetCollisionCenter());
        RaiseValueChangedEvent(mousePos);
    }
}
void GUISlider::OnMouseDrag(Vector2f originalPos, Vector2f currentPos)
{
    if (IsClickable && (IsLocalInsideBounds(originalPos) || IsLocalInsideBounds(currentPos)))
    {
        Vector2f dims = GetCollisionDimensions();
        Value = GetNewValue(currentPos);

        if (UsesTimeLerp() && CurrentTimeLerpSpeed <= 0.0f)
        {
            CurrentTimeLerpSpeed = TimeLerpSpeed;
            SetTimeLerp(0.0f);
        }

        RaiseValueChangedEvent(currentPos);
    }
}