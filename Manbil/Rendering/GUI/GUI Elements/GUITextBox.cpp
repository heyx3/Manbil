#include "GUITextBox.h"



GUITextBox::GUITextBox(const GUITexture & box, const GUITexture & cursor,
                       const GUILabel & boxContents, bool editable, float lerpSpeed)
    : Editable(editable), Box(box), Cursor(cursor),
      Contents(boxContents), isSelected(false), GUIElement(UniformDictionary(), lerpSpeed)
{
    Contents.SetOffsetHorz(GUILabel::HO_LEFT);
    Contents.SetOffsetVert(GUILabel::VO_CENTER);

    
    Box.Depth = 0.0f;
    Contents.Depth = 0.00001f;
    Cursor.Depth = 0.00002f;


    keyboardInput.ClearText(false);
    keyboardInput.InsertText(0, Contents.GetText());

    keyboardInput.OnEnterKey_Data = this;
    keyboardInput.OnEnterKey = [](KeyboardTextInput * thisInput, void* pData)
    {
        GUITextBox * box = ((GUITextBox*)pData);
        if (box->OnPressEnter != 0) box->OnPressEnter(box, box->OnPressEnter_Data);
    };

    keyboardInput.OnTextChanged_Data = this;
    keyboardInput.OnTextChanged = [](KeyboardTextInput * thisInput, void* pData)
    {
        GUITextBox * box = ((GUITextBox*)pData);
        box->Contents.SetText(thisInput->GetText());
        box->DidBoundsChange = true;
        if (box->OnTextChanged != 0)
            box->OnTextChanged(box, box->OnTextChanged_Data);
    };
}
GUITextBox::GUITextBox(const GUITextBox & cpy)
    : Editable(cpy.Editable), Box(cpy.Box), Cursor(cpy.Cursor),
      Contents(cpy.Contents), isSelected(false), GUIElement(cpy.Params, cpy.GetTimeLerp())
{
    cpy.keyboardInput.CopyTo(keyboardInput);

    keyboardInput.OnEnterKey_Data = this;
    keyboardInput.OnTextChanged_Data = this;
}


Box2D GUITextBox::GetBounds(void) const
{
    return Box.GetBounds();
}

void GUITextBox::SetScale(Vector2f newScale)
{
    Vector2f deltaScale(newScale.x / GetScale().x, newScale.y / GetScale().y);

    GUIElement::SetScale(newScale);

    Box.SetScale(newScale);
    Contents.ScaleBy(deltaScale);
}

std::string GUITextBox::SetText(const std::string & newStr)
{
    keyboardInput.ClearText(true);
    keyboardInput.InsertText(0, newStr);

    if (!Contents.SetText(newStr))
        return "Error changing text box contents to '" + newStr + "': " +
                   Contents.GetTextRenderer()->GetError();
    return "";
}

void GUITextBox::CustomUpdate(float elapsed, Vector2f mousePos)
{
    keyboardInput.OnEnterKey_Data = this;
    keyboardInput.OnTextChanged_Data = this;
    if (isSelected && Editable)
        keyboardInput.Update(elapsed);

    DidBoundsChange = (Box.IsValid() && Box.DidBoundsChange) || DidBoundsChange;

    if (Box.IsValid()) Box.Update(elapsed, mousePos);
    if (Cursor.IsValid()) Cursor.Update(elapsed, mousePos - Cursor.GetPos());
}
std::string GUITextBox::Render(float elapsedTime, const RenderInfo & info)
{
    std::string err = "";
    Vector4f myCol = GetColor();

    //Some positioning data.
    Box2D textBounds = Contents.GetBounds(),
          boxBounds = Box.GetBounds();

    //Render the box.
    err = RenderChild(&Box, elapsedTime, info);
    if (!err.empty()) return "Error rendering box: " + err;

    //Render the text.
    err = RenderChild(&Contents, elapsedTime, info);
    if (!err.empty()) return "Error rendering text: " + err;

    //If the cursor is on, render it.
    if (isSelected)
    {
        float cursorX;
        if (GetText().empty())
        {
            cursorX = boxBounds.GetXMin();
        }
        else
        {
            cursorX = BasicMath::Lerp(boxBounds.GetXMin(), boxBounds.GetXMin() + textBounds.GetXSize(),
                                      (float)keyboardInput.CursorPos / (float)GetText().size());
        }
        Cursor.SetPosition(Vector2f(cursorX, Cursor.GetPos().y));

        err = RenderChild(&Cursor, elapsedTime, info);
        if (!err.empty()) return "Error rendering cursor: " + err;
    }

    return "";
}

void GUITextBox::OnMouseClick(Vector2f relativeMousePos)
{
    if (!Editable) return;

    Box2D bounds = Box.GetBounds();

    bool isInside = bounds.IsPointInside(relativeMousePos);
    if (isInside)
    {
        isSelected = true;

        float inverseLerp = BasicMath::LerpComponent(bounds.GetXMin(),
                                                     bounds.GetXMin() + Contents.GetBounds().GetXSize(),
                                                     relativeMousePos.x);
        if (inverseLerp < 0.5f)
            keyboardInput.CursorPos = 0;
        else keyboardInput.CursorPos = keyboardInput.GetText().size();
    }
    else isSelected = false;
}