#pragma once

#include "GUILabel.h"
#include "GUITexture.h"
#include "../../../Input/KeyboardTextInput.h"


//A text box.
class GUITextBox : public GUIElement
{
public:

    bool Editable;
    GUITexture Box, Cursor, Highlight;
    GUILabel Contents;
    float Width;

    Vector2f Center;


    //Called when the user presses the Enter key while this text box is selected.
    //The "pData" value passed in is this instance's "OnPressEnter_Data" field.
    void (*OnPressEnter)(GUITextBox * thisBox, void* pData) = 0;
    void* OnPressEnter_Data = 0;

    //Called when this box's text contents are changed.
    //The "pData" value passed in is this instance's "OnEnterText_Data" field.
    void(*OnTextChanged)(GUITextBox * thisBox, void* pData) = 0;
    void* OnTextChanged_Data = 0;


    GUITextBox(const GUITexture & box, const GUITexture & cursor, const GUITexture & highlight,
               const GUILabel & boxContents, float width,
               bool editable, const UniformDictionary & params, float lerpSpeed = 1.0f);
    GUITextBox(void) : GUIElement(UniformDictionary()), Center(), Width(0.0f) { }
    GUITextBox(const GUITextBox & cpy);

    const std::string & GetText(void) const { return Contents.GetText(); }
    TextRenderer::FontSlot GetFontSlot(void) const { return Contents.TextRenderSlot; }


    virtual Vector2f GetCollisionCenter(void) const override { return Center; }
    virtual Vector2f GetCollisionDimensions(void) const;

    virtual void MoveElement(Vector2f moveAmount) override { Center += moveAmount; }
    virtual void SetPosition(Vector2f newPos) override { Center = newPos; }

    virtual Vector2f GetScale(void) const override { return scale; }
    virtual void ScaleBy(Vector2f scaleAmount) override;
    virtual void SetScale(Vector2f newScale) override;


    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;

    virtual void OnMouseClick(Vector2f relativeMousePos) override;


protected:

    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) override;


private:

    bool isSelected;
    Vector2f scale;
    KeyboardTextInput keyboardInput;
};