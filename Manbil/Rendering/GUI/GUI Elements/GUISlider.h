#pragma once

#include "../GUIElement.h"


//A bar with a slibing nub that represents a value between 0 and 1.
class GUISlider : public GUIElement
{
public:

    MTexture2D *Bar, *Nub;
    Material *BarMat, *NubMat;

    Vector2f BarScale, NubScale;

    float Value;
    bool IsClickable;

    //If false, this slider is horizontal.
    bool IsVertical;

    void(*OnValueChanged)(GUISlider * changed, Vector2i localMouse, void* pData) = 0;
    void *OnValueChanged_pData = 0;


    virtual Vector2i GetCollisionCenter(void) const override { return center; }
    virtual Vector2i GetCollisionDimensions(void) const override;

    virtual void MoveElement(Vector2i moveAmount) override { center += moveAmount; }
    virtual void SetPosition(Vector2i newPos) override { center = newPos; }

    virtual void ScaleBy(Vector2f scaleAmount) override { BarScale.MultiplyComponents(scaleAmount); NubScale.MultiplyComponents(scaleAmount); }


    GUISlider(MTexture2D* bar, MTexture2D* nub,
              Material* barMat, Material* nubMat,
              Vector2f barScale = Vector2f(1.0f, 1.0f), Vector2f nubScale = Vector2f(1.0f, 1.0f),
              bool isClickable = true, bool isVertical = false, float timeLerpSpeed = 1.0f)
              : GUIElement(timeLerpSpeed), Bar(bar), Nub(nub), IsClickable(isClickable), IsVertical(isVertical),
                NubMat(nubMat), BarMat(barMat), BarScale(barScale), NubScale(nubScale)
    {

    }

    
    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;

    virtual void OnMouseClick(Vector2i mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2i originalPos_centerOffset,
                             Vector2i mouse_centerOffset) override;


private:

    Vector2i center;
};