#pragma once

#include "GUITexture.h"


//Like a toggle-able button: click to turn on, then click again to turn off.
class GUICheckbox : public GUIElement
{
public:

    GUITexture Box, Check;

    Vector2f Center, scale;

    //If true, then the "Check" texture will be drawn IN PLACE OF the "Box" texture,
    //    instead of being drawn on top of it.
    bool HideBoxIfChecked;

    //Raised when this element is checked or unchecked.
    //The "pData" argument is the value of this instance's "OnClicked_Data" field.
    void(*OnClicked)(GUICheckbox * thisBox, void* pData) = 0;
    void* OnClicked_Data = 0;


    GUICheckbox(const UniformDictionary & params, const GUITexture & box, const GUITexture & check,
                bool hideBoxIfChecked = false, float timeLerpSpeed = 1.0f)
        : Box(box), Check(check), HideBoxIfChecked(hideBoxIfChecked),
          scale(1.0f, 1.0f), GUIElement(params, timeLerpSpeed)
    {

    }
    GUICheckbox(void) : GUICheckbox(UniformDictionary(), GUITexture(), GUITexture()) { }


    bool IsChecked(void) const { return isChecked; }
    void ToggleCheck(bool raiseEvent) { isChecked = !isChecked; if (raiseEvent) RaiseOnClickedEvent(); }
    void SetChecked(bool newVal, bool raiseEvent) { isChecked = newVal; if (raiseEvent) RaiseOnClickedEvent(); }


    virtual Vector2f GetCollisionCenter(void) const override { return Center; }
    virtual Vector2f GetCollisionDimensions(void) const override;

    virtual void MoveElement(Vector2f moveAmount) override { Center += moveAmount; }
    virtual void SetPosition(Vector2f newPos) override { Center = newPos; }

    virtual Vector2f GetScale(void) const override { return scale; }
    virtual void ScaleBy(Vector2f scaleAmount) override { scale.MultiplyComponents(scaleAmount); Box.ScaleBy(scaleAmount); Check.ScaleBy(scaleAmount); }
    virtual void SetScale(Vector2f newScale) override;

    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;

    virtual void OnMouseClick(Vector2f relativeMousePos) override;
    virtual void OnMouseDrag(Vector2f originalMousePos, Vector2f newMousePos) override;
    virtual void OnMouseRelease(Vector2f newMousePos) override;

protected:

    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) override;

private:

    std::string RenderBox(float elapsed, Vector4f myCol, const RenderInfo & info);
    std::string RenderCheck(float elapsed, float depth, Vector4f myCol, const RenderInfo & info);

    void RaiseOnClickedEvent(void) { if (OnClicked != 0) OnClicked(this, OnClicked_Data); }

    bool isChecked = false;
};