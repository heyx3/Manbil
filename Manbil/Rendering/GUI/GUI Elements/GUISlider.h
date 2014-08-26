#pragma once

#include "../GUIElement.h"
#include "GUITexture.h"


//A bar with a slibing nub that represents a value between 0 and 1.
class GUISlider : public GUIElement
{
public:

    GUITexture Bar, Nub;

    float Value;
    bool IsClickable;

    //If false, this slider is horizontal.
    bool IsVertical;

    void(*OnValueChanged)(GUISlider * changed, Vector2f localMouse, void* pData) = 0;
    void *OnValueChanged_pData = 0;


    virtual Vector2f GetCollisionCenter(void) const override { return center; }
    virtual Vector2f GetCollisionDimensions(void) const override;

    virtual void MoveElement(Vector2f moveAmount) override { center += moveAmount; }
    virtual void SetPosition(Vector2f newPos) override { center = newPos; }

    virtual Vector2f GetScale(void) const override { return Bar.GetScale(); }

    virtual void ScaleBy(Vector2f scaleAmount) override { Bar.ScaleBy(scaleAmount); Nub.ScaleBy(scaleAmount); }
    virtual void SetScale(Vector2f newScale) override;


    GUISlider(const UniformDictionary & params,
              MTexture2D* bar = 0, MTexture2D* nub = 0,
              Material* barMat = 0, Material* nubMat = 0,
              Vector2f barScale = Vector2f(1.0f, 1.0f), Vector2f nubScale = Vector2f(1.0f, 1.0f),
              bool isClickable = true, bool isVertical = false, float timeLerpSpeed = 1.0f)
              : GUIElement(params, timeLerpSpeed), IsClickable(isClickable), IsVertical(isVertical),
                Bar(params, bar, barMat, false, timeLerpSpeed), Nub(params, nub, nubMat, false, timeLerpSpeed)
    {
        Bar.Scale = barScale;
        Nub.Scale = nubScale;
    }

    
    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;

    virtual void OnMouseClick(Vector2f mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2f originalPos_centerOffset,
                             Vector2f mouse_centerOffset) override;


private:

    float GetNewValue(Vector2f mousePos) const;

    Vector2f center;
};