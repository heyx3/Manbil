#include "GUITextBox.h"



GUITextBox::GUITextBox(const GUITexture & box, const GUITexture & cursor, const GUITexture & highlight,
                       const GUILabel & boxContents, float width,
                       bool editable, const UniformDictionary & params, float lerpSpeed)
    : Editable(editable), Box(box), Cursor(cursor), Highlight(highlight),
      Contents(boxContents), Center(0.0f, 0.0f), Width(width), scale(1.0f, 1.0f),
      isSelected(false), GUIElement(params, lerpSpeed)
{
    Contents.OffsetHorz = GUILabel::HO_LEFT;
    Contents.OffsetVert = GUILabel::VO_CENTER;


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
        if (box->OnTextChanged != 0)
            box->OnTextChanged(box, box->OnTextChanged_Data);
    };
}
GUITextBox::GUITextBox(const GUITextBox & cpy)
    : Editable(cpy.Editable), Box(cpy.Box), Cursor(cpy.Cursor), Highlight(cpy.Highlight),
      Contents(cpy.Contents), Center(cpy.Center), Width(cpy.Width), scale(cpy.scale),
      isSelected(false), GUIElement(cpy.Params, cpy.GetTimeLerp())
{
    cpy.keyboardInput.CopyTo(keyboardInput);

    keyboardInput.OnEnterKey_Data = this;
    keyboardInput.OnTextChanged_Data = this;
}


Vector2f GUITextBox::GetCollisionDimensions(void) const
{
    return scale.ComponentProduct(Vector2f(Width, Contents.GetCollisionDimensions().y));
}

void GUITextBox::ScaleBy(Vector2f scaleAmount)
{
    scale.MultiplyComponents(scaleAmount);
    Box.ScaleBy(scaleAmount);
    Contents.ScaleBy(scaleAmount);
}
void GUITextBox::SetScale(Vector2f newScale)
{
    Vector2f deltaScale(newScale.x / scale.x, newScale.y / scale.y);
    scale = newScale;
    Box.SetScale(newScale);
    Contents.ScaleBy(deltaScale);
}

void GUITextBox::CustomUpdate(float elapsed, Vector2f mousePos)
{
    keyboardInput.OnEnterKey_Data = this;
    keyboardInput.OnTextChanged_Data = this;
    if (isSelected && Editable)
        keyboardInput.Update(elapsed);

    if (Box.IsValid()) Box.Update(elapsed, mousePos);
    if (Cursor.IsValid()) Cursor.Update(elapsed, mousePos - Cursor.GetCollisionCenter());
    if (Highlight.IsValid()) Highlight.Update(elapsed, mousePos - Highlight.GetCollisionCenter());
}
std::string GUITextBox::Render(float elapsedTime, const RenderInfo & info)
{
    std::string err = "";
    Vector4f myCol = GetColor();

    //Some positioning data.
    Vector2f textBounds = Contents.GetCollisionDimensions();
    float halfWidth = Width * scale.x * 0.5f;
    Vector2f nCenter = -Center;

    //Render the box.
    Box.SetBounds(Vector2f(-halfWidth, textBounds.y * -0.5f),
                  Vector2f(halfWidth, textBounds.y * 0.5f));
    Box.Depth = Depth;
    Vector4f oldCol = Box.GetColor();
    Box.SetColor(oldCol.ComponentProduct(myCol));
    Box.MoveElement(Center);
    err = Box.Render(elapsedTime, info);

    Box.MoveElement(nCenter);
    Box.SetColor(oldCol);
    if (!err.empty()) return "Error rendering box: " + err;

    //Render the text.
    Contents.SetPosition(Vector2f(-halfWidth, 0.0f));
    Contents.Depth = Depth + 0.00001f;
    Contents.MoveElement(Center);
    oldCol = Contents.GetColor();
    Contents.SetColor(oldCol.ComponentProduct(myCol));
    err = Contents.Render(elapsedTime, info);
    Contents.SetColor(oldCol);
    Contents.MoveElement(nCenter);
    if (!err.empty()) return "Error rendering text: " + err;

    //If the cursor is on, render it.
    if (isSelected)
    {
        float cursorX = BasicMath::Lerp(-halfWidth, -halfWidth + textBounds.x,
                                        (float)keyboardInput.CursorPos / (float)GetText().size());
        float cWidth = Cursor.GetCollisionDimensions().x;
        Cursor.SetBounds(Vector2f(cursorX - (cWidth * 0.5f), textBounds.y * -0.5f),
                         Vector2f(cursorX + (cWidth * 0.5f), textBounds.y * 0.5f));
        Cursor.Depth = Depth + 0.00002f;
        Cursor.MoveElement(Center);
        oldCol = Cursor.GetColor();
        Cursor.SetColor(oldCol.ComponentProduct(myCol));
        err = Cursor.Render(elapsedTime, info);
        Cursor.MoveElement(nCenter);
        Cursor.SetColor(oldCol);
        if (!err.empty()) return "Error rendering cursor: " + err;
    }

    return "";
}

void GUITextBox::OnMouseClick(Vector2f relativeMousePos)
{
    if (!Editable) return;

    bool isInside = IsLocalInsideBounds(relativeMousePos);
    if (isInside)
    {
        isSelected = true;

        float halfWidth = Width * scale.x * 0.5f;
        float inverseLerp = BasicMath::LerpComponent(-halfWidth,
                                                     -halfWidth + Contents.GetCollisionDimensions().x,
                                                     relativeMousePos.x);
        if (inverseLerp < 0.5f)
            keyboardInput.CursorPos = 0;
        else keyboardInput.CursorPos = keyboardInput.GetText().size();
    }
    else isSelected = false;
}