#include "GUISlider.h"


GUISlider::GUISlider(const UniformDictionary& params,
                     const GUITexture& barTex, const GUITexture& nubTex, float startingValLerp,
                     bool isClickable, bool isVertical, float timeLerpSpeed)
    : GUIElement(params, timeLerpSpeed), IsClickable(isClickable), IsVertical(isVertical),
      Bar(barTex), Nub(nubTex), Value(startingValLerp)
{
}

bool GUISlider::GetDidBoundsChangeDeep(void) const
{
    return DidBoundsChange || Bar.DidBoundsChange;
}
void GUISlider::ClearDidBoundsChangeDeep(void)
{
    DidBoundsChange = false;
    Bar.DidBoundsChange = false;
}

void GUISlider::ScaleBy(Vector2f scaleAmount)
{
    SetScale(scaleAmount.ComponentProduct(GetScale()));
}    

Box2D GUISlider::GetBounds(void) const
{
    //The length along the slider axis is the length of the slider plus
    //    the length of half the nub on each side.
    //The length perpendicular to the slider axis is the length of the bar/nub -- whichever is longest.

    Box2D barBounds = Bar.GetBounds(),
          nubBounds = Nub.GetBounds();
    if (IsVertical)
    {
        return Box2D(Bar.GetPos(),
                     Vector2f(Mathf::Max(barBounds.GetXSize(), nubBounds.GetXSize()),
                              barBounds.GetYSize() + nubBounds.GetYSize()));
    }
    else
    {
        return Box2D(Bar.GetPos(),
                     Vector2f(barBounds.GetXSize() + nubBounds.GetXSize(),
                              Mathf::Max(barBounds.GetYSize(), nubBounds.GetYSize())));
    }
}


void GUISlider::SetScale(Vector2f newScale)
{
    Vector2f oldScale = GetScale();
    Vector2f deltaScale(newScale.x / oldScale.x, newScale.y / oldScale.y);

    Bar.ScaleBy(deltaScale);
    Nub.ScaleBy(deltaScale);

    GUIElement::SetScale(newScale);
}

float GUISlider::GetNewValue(Vector2f mousePos) const
{
    Box2D barBounds = Bar.GetBounds();

    if (IsVertical)
    {
        return Mathf::Clamp(Mathf::LerpComponent(barBounds.GetYMin(),
                                                 barBounds.GetYMax(),
                                                 mousePos.y),
                            0.0f, 1.0f);
    }
    else
    {
        return Mathf::Clamp(Mathf::LerpComponent(barBounds.GetXMin(),
                                                 barBounds.GetXMax(),
                                                 mousePos.x),
                            0.0f, 1.0f);
    }
}

void GUISlider::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    Bar.Update(elapsed, relativeMousePos - Bar.GetPos());
    Nub.Update(elapsed, relativeMousePos - Nub.GetPos());
}
void GUISlider::Render(float elapsedTime, const RenderInfo & info)
{
    //Render bar.
    RenderChild(&Bar, elapsedTime, info);


    //Render nub. Keep the nub's "DidBoundsChange" boolean from being effected during rendering,
    //    since repositioning the nub will never have an effect on the slider's bounds.

    Box2D barBounds = Bar.GetBounds();
    bool oldNubChanged = Nub.DidBoundsChange;

    Vector2f nubPos;
    if (IsVertical)
    {
        nubPos = Vector2f(barBounds.GetCenterX(), barBounds.GetYInterval().Lerp(Value));
    }
    else
    {
        nubPos = Vector2f(barBounds.GetXInterval().Lerp(Value), barBounds.GetCenterY());
    }
    Nub.SetPosition(nubPos);

    RenderChild(&Nub, elapsedTime, info);
    Nub.DidBoundsChange = oldNubChanged;
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

void GUISlider::RaiseValueChangedEvent(Vector2f localMouse)
{
    if (OnValueChanged != 0)
    {
        OnValueChanged(this, localMouse, OnValueChanged_pData);
    }
}