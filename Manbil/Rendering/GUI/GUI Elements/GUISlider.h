#pragma once

#include "../GUIElement.h"
#include "GUITexture.h"


//A bar with a slibing nub that represents a value between 0 and 1.
class GUISlider : public GUIElement
{
public:

    GUITexture Bar;

    //The nub's position is set by this GUISlider every Render() call;
    //    setting it externally won't do anything.
    GUITexture Nub;

    float Value;
    bool IsClickable;

    //If false, this slider is horizontal.
    bool IsVertical;

    void(*OnValueChanged)(GUISlider * changed, Vector2f localMouse, void* pData) = 0;
    void *OnValueChanged_pData = 0;


    GUISlider(const UniformDictionary & params,
              const GUITexture& barTex, const GUITexture& nubTex,
              bool isClickable = true, bool isVertical = false, float timeLerpSpeed = 1.0f)
              : GUIElement(params, timeLerpSpeed), IsClickable(isClickable), IsVertical(isVertical),
                Bar(barTex), Nub(nubTex) { }
    GUISlider(void) : GUISlider(UniformDictionary(), GUITexture(), GUITexture()) { }
    

    virtual Box2D GetBounds(void) const override;

    virtual void ScaleBy(Vector2f scaleAmount) override { SetScale(scaleAmount.ComponentProduct(GetScale())); }
    virtual void SetScale(Vector2f newScale) override;

    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) override;
    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;

    virtual void OnMouseClick(Vector2f mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2f originalPos_centerOffset,
                             Vector2f mouse_centerOffset) override;


private:

    float GetNewValue(Vector2f mousePos) const;
    void RaiseValueChangedEvent(Vector2f localMouse) { if (OnValueChanged != 0) OnValueChanged(this, localMouse, OnValueChanged_pData); }
};