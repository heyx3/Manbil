#include "GUITextBox.h"



GUITextBox::GUITextBox(const GUITexture& box, const GUITexture& cursor,
                       const GUILabel& boxContents, bool editable, float lerpSpeed)
    : Editable(editable), Box(box), Cursor(cursor),
      Contents(boxContents), isSelected(false),
      GUIElement(UniformDictionary(), lerpSpeed)
{
    Contents.SetOffsetHorz(GUILabel::HO_LEFT);
    Contents.SetOffsetVert(GUILabel::VO_CENTER);
    
    Box.Depth = 0.0f;
    Contents.Depth = 0.00001f;
    Cursor.Depth = 0.00002f;


    keyboardInput.ClearText(false);
    keyboardInput.InsertText(0, Contents.GetText());

    keyboardInput.OnEnterKey_Data = this;

#pragma warning(disable: 4100)
    keyboardInput.OnEnterKey = [](KeyboardTextInput* thisInput, void* pData)
    {
        GUITextBox* box = ((GUITextBox*)pData);
        if (box->OnPressEnter != 0)
        {
            box->OnPressEnter(box, box->OnPressEnter_Data);
        }
    };
#pragma warning(default: 4100)

    keyboardInput.OnTextChanged_Data = this;
    
#pragma warning(disable: 4100)
    keyboardInput.OnTextChanged = [](KeyboardTextInput* thisInput, void* pData)
    {
        GUITextBox* box = ((GUITextBox*)pData);
        box->Contents.SetText(thisInput->GetText());
        box->DidBoundsChange = true;
        if (box->OnTextChanged != 0)
        {
            box->OnTextChanged(box, box->OnTextChanged_Data);
        }
    };
#pragma warning(default: 4100)
}
GUITextBox::GUITextBox(const GUITextBox& cpy)
    : Editable(cpy.Editable), Box(cpy.Box), Cursor(cpy.Cursor),
      Contents(cpy.Contents), isSelected(false), GUIElement(cpy.Params, cpy.GetTimeLerp())
{
    cpy.keyboardInput.CopyTo(keyboardInput);

    keyboardInput.OnEnterKey_Data = this;
    keyboardInput.OnTextChanged_Data = this;
}


Box2D GUITextBox::GetBounds(void) const
{
    Box2D bnds = Box.GetBounds();
    bnds.Move(Box.GetPos());
    return bnds;
}

bool GUITextBox::GetDidBoundsChangeDeep(void) const
{
    return DidBoundsChange || (Box.IsValid() && Box.DidBoundsChange);
}
void GUITextBox::ClearDidBoundsChangeDeep(void)
{
    DidBoundsChange = false;
    Box.DidBoundsChange = false;
}

void GUITextBox::ScaleBy(Vector2f scaleAmount)
{
    SetScale(GetScale().ComponentProduct(scaleAmount));
}

void GUITextBox::SetScale(Vector2f newScale)
{
    Vector2f deltaScale(newScale.x / GetScale().x, newScale.y / GetScale().y);

    GUIElement::SetScale(newScale);

    Box.ScaleBy(deltaScale);
    Contents.ScaleBy(deltaScale);
}

std::string GUITextBox::SetText(const std::string& newStr)
{
    keyboardInput.ClearText(true);
    keyboardInput.InsertText(0, newStr);

    if (!Contents.SetText(newStr))
    {
        return "Error changing text box contents to '" + newStr + "'";
    }

    return "";
}

void GUITextBox::CustomUpdate(float elapsed, Vector2f mousePos)
{
    keyboardInput.OnEnterKey_Data = this;
    keyboardInput.OnTextChanged_Data = this;
    if (isSelected && Editable)
    {
        keyboardInput.Update(elapsed);
    }

    if (Box.IsValid())
    {
        Box.Update(elapsed, mousePos);
    }
    if (Cursor.IsValid())
    {
        Cursor.Update(elapsed, mousePos - Cursor.GetPos());
    }
}
void GUITextBox::Render(float elapsedTime, const RenderInfo& info)
{
    std::string err = "";
    Vector4f myCol = GetColor();

    //Some positioning data.
    Box2D textBounds = Contents.GetBounds(),
          boxBounds = Box.GetBounds();

    //Render the box.
    RenderChild(&Box, elapsedTime, info);

    //Render the text.
    Contents.SetPosition(Vector2f(boxBounds.GetXMin(), boxBounds.GetCenterY()));
    RenderChild(&Contents, elapsedTime, info);

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
            cursorX = Mathf::Lerp(boxBounds.GetXMin(), boxBounds.GetXMin() + textBounds.GetXSize(),
                                  (float)keyboardInput.CursorPos / (float)GetText().size());
        }
        Cursor.SetPosition(Vector2f(cursorX, Cursor.GetPos().y));

        RenderChild(&Cursor, elapsedTime, info);
    }
}

void GUITextBox::OnMouseClick(Vector2f relativeMousePos)
{
    if (!Editable)
    {
        return;
    }

    Box2D bounds = Box.GetBounds();

    bool isInside = bounds.IsPointInside(relativeMousePos);
    if (isInside)
    {
        isSelected = true;

        float inverseLerp = Mathf::LerpComponent(bounds.GetXMin(),
                                                 bounds.GetXMin() + Contents.GetBounds().GetXSize(),
                                                 relativeMousePos.x);
        if (inverseLerp < 0.5f)
        {
            keyboardInput.CursorPos = 0;
        }
        else
        {
            keyboardInput.CursorPos = keyboardInput.GetText().size();
        }
    }
    else isSelected = false;
}