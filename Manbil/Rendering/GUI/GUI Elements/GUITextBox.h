#pragma once

#include "GUILabel.h"
#include "GUITexture.h"
#include "../../../Input/KeyboardTextInput.h"


//A text box that can optionally be edited by the user.
class GUITextBox : public GUIElement
{
public:

    bool Editable;
    GUITexture Box, Cursor;
    GUILabel Contents;


    //Called when the user presses the Enter key while this text box is selected.
    //The "pData" value passed in is this instance's "OnPressEnter_Data" field.
    void (*OnPressEnter)(GUITextBox * thisBox, void* pData) = 0;
    void* OnPressEnter_Data = 0;

    //Called when this box's text contents are changed.
    //The "pData" value passed in is this instance's "OnEnterText_Data" field.
    void(*OnTextChanged)(GUITextBox * thisBox, void* pData) = 0;
    void* OnTextChanged_Data = 0;


    GUITextBox(const GUITexture & box, const GUITexture & cursor,
               const GUILabel & boxContents, bool editable, float lerpSpeed = 1.0f);
    GUITextBox(void) : GUIElement(UniformDictionary()) { }
    GUITextBox(const GUITextBox & cpy);

    const std::string & GetText(void) const { return Contents.GetText(); }
    TextRenderer::FontSlot GetFontSlot(void) const { return Contents.GetTextRenderSlot(); }

    //Returns an error message, or the empty string if everything went fine.
    std::string SetText(const std::string & newString);


    virtual Box2D GetBounds(void) const override;

    virtual bool GetDidBoundsChangeDeep(void) const override
    {
        return DidBoundsChange || (Box.IsValid() && Box.DidBoundsChange);
    }
    virtual void ClearDidBoundsChangeDeep(void) override { DidBoundsChange = false; Box.DidBoundsChange = false; }

    virtual void ScaleBy(Vector2f scaleAmount) override { SetScale(GetScale().ComponentProduct(scaleAmount)); }
    virtual void SetScale(Vector2f newScale) override;

    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;

    virtual void OnMouseClick(Vector2f relativeMousePos) override;


protected:

    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) override;


private:

    bool isSelected;
    KeyboardTextInput keyboardInput;
};