#include "GUISlider.h"


Box2D GUISlider::GetBounds(void) const
{
    //The length along the slider axis is the length of the slider plus the length of half the nub on each side.
    //The length perpendicular to the slider axis is the length of the bar/nub -- whichever is longest.

    Box2D barBounds = Bar.GetBounds(),
          nubBounds = Nub.GetBounds();
    if (IsVertical)
    {
        return Box2D(barBounds.GetCenter(),
                     Vector2f(BasicMath::Max(barBounds.GetXSize(), nubBounds.GetXSize()),
                              barBounds.GetYSize() + nubBounds.GetYSize()));
    }
    else
    {
        return Box2D(barBounds.GetCenter(),
                     Vector2f(barBounds.GetXSize() + nubBounds.GetXSize(),
                              BasicMath::Max(barBounds.GetYSize(), nubBounds.GetYSize())));
    }
}


void GUISlider::SetScale(Vector2f newScale)
{
    Vector2f oldBarScale = Bar.GetScale();
    Vector2f deltaScale(newScale.x / oldBarScale.x, newScale.y / oldBarScale.y);

    Bar.SetScale(newScale);
    Nub.ScaleBy(deltaScale);

    GUIElement::SetScale(newScale);
}

float GUISlider::GetNewValue(Vector2f mousePos) const
{
    Box2D barBounds = Bar.GetBounds();

    if (IsVertical)
    {
        return BasicMath::Clamp(BasicMath::LerpComponent(barBounds.GetYMin(), barBounds.GetYMax(),
                                                         mousePos.y),
                                0.0f, 1.0f);
    }
    else
    {
        return BasicMath::Clamp(BasicMath::LerpComponent(barBounds.GetXMin(), barBounds.GetXMax(),
                                                         mousePos.x),
                                0.0f, 1.0f);
    }
}

void GUISlider::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    Bar.Update(elapsed, relativeMousePos);
    Nub.Update(elapsed, relativeMousePos);
}
std::string GUISlider::Render(float elapsedTime, const RenderInfo & info)
{
    std::string err;


    //Render bar.
    err = RenderChild(&Bar, elapsedTime, info);
    if (!err.empty()) return "Error rendering bar: " + err;


    //Render nub. Keep the nub's "DidBoundsChange" boolean from being effected during rendering,
    //    since repositioning the nub will never have an effect on the slider's bounds.

    Box2D barBounds = Bar.GetBounds();
    bool oldNubChanged = Nub.DidBoundsChange;

    Vector2f nubPos;
    if (IsVertical)
    {
        nubPos = Vector2f(barBounds.GetCenterX(), barBounds.GetYInterval().RangeLerp(Value));
    }
    else
    {
        nubPos = Vector2f(barBounds.GetXInterval().RangeLerp(Value), barBounds.GetCenterY());
    }
    Nub.SetPosition(nubPos);

    err = RenderChild(&Nub, elapsedTime, info);
    Nub.DidBoundsChange = oldNubChanged;
    if (!err.empty()) return "Error rendering nub: " + err;


    return "";
}

void GUISlider::OnMouseClick(Vector2f mousePos)
{
    if (IsClickable && GetBounds().IsPointInside(mousePos))
    {
        Value = GetNewValue(mousePos);
        
        if (UsesTimeLerp())
        {
            CurrentTimeLerpSpeed = TimeLerpSpeed;
            SetTimeLerp(0.0f);
        }

        Nub.OnMouseClick(mousePos - Nub.GetPos());
        Bar.OnMouseClick(mousePos - Bar.GetPos());
        RaiseValueChangedEvent(mousePos);
    }
}
void GUISlider::OnMouseDrag(Vector2f originalPos, Vector2f currentPos)
{
    Box2D bounds = GetBounds();
    if (IsClickable && (bounds.IsPointInside(originalPos) || bounds.IsPointInside(currentPos)))
    {
        Value = GetNewValue(currentPos);

        if (UsesTimeLerp() && CurrentTimeLerpSpeed <= 0.0f)
        {
            CurrentTimeLerpSpeed = TimeLerpSpeed;
            SetTimeLerp(0.0f);
        }

        RaiseValueChangedEvent(currentPos);
    }
}