#pragma once

#include "GUITexture.h"


//Like a toggle-able button: click to turn on, then click again to turn off.
class GUICheckbox : public GUIElement
{
public:

    GUITexture Box, Check;

    //If true, then the "Check" texture will be drawn IN PLACE OF the "Box" texture,
    //    instead of being drawn on top of it.
    bool HideBoxIfChecked;

    //Raised when this element is checked or unchecked.
    //The "pData" argument is the value of this instance's "OnClicked_Data" field.
    void(*OnClicked)(GUICheckbox * thisBox, void* pData) = 0;
    void* OnClicked_Data = 0;


    GUICheckbox(void) : GUICheckbox(GUITexture(), GUITexture()) { }
    GUICheckbox(const GUITexture& box, const GUITexture& check,
                bool hideBoxIfChecked = false, float timeLerpSpeed = 1.0f);


    bool IsChecked(void) const { return isChecked; }
    void ToggleCheck(bool raiseEvent);
    void SetChecked(bool newVal, bool raiseEvent);


    virtual Box2D GetBounds(void) const override;
    virtual bool GetDidBoundsChangeDeep(void) const override;
    virtual void ClearDidBoundsChangeDeep(void) override;
    virtual void ScaleBy(Vector2f scaleAmount) override;
    virtual void SetScale(Vector2f newScale) override;

    virtual void Render(float elapsedTime, const RenderInfo& info) override;

    virtual void OnMouseClick(Vector2f relativeMousePos) override;
    virtual void OnMouseDrag(Vector2f originalMousePos, Vector2f newMousePos) override;
    virtual void OnMouseRelease(Vector2f newMousePos) override;


protected:

    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) override;

private:

    std::string RenderBox(float elapsed, Vector4f myCol, const RenderInfo& info);
    std::string RenderCheck(float elapsed, float depth, Vector4f myCol, const RenderInfo& info);

    void RaiseOnClickedEvent(void);

    bool isChecked = false;
};