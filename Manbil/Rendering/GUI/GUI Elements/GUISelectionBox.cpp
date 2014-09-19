#include "GUISelectionBox.h"


GUISelectionBox::GUISelectionBox(const UniformDictionary & params,
                                 TextRenderer* textRender, const GUITexture & boxElement,
                                 unsigned int fontID, Vector2u fontRenderTexSize,
                                 const TextureSampleSettings2D & fontRenderSettings,
                                 Material * itemTextMat, GUILabel::HorizontalOffsets textOffset,
                                 const GUITexture & itemListBackground, 
                                 const std::vector<std::string> & _items, unsigned int selected,
                                 bool extendAbove, float timeLerpSpeed)
    : GUIElement(params, timeLerpSpeed), TextRender(textRender),
      BoxElement(boxElement), ExtendAbove(extendAbove), selectedItem(selected),
      scale(1.0f, 1.0f), itemBackground(itemListBackground), itemFontID(fontID), IsExtended(false)
{
    unsigned int firstSlotIndex = TextRender->GetNumbSlots(fontID);
    bool tryCre = TextRender->CreateTextRenderSlots(fontID, fontRenderTexSize.x, fontRenderTexSize.y, false,
                                                    fontRenderSettings, _items.size());
    if (!tryCre)
    {
        BoxElement.Mat = 0;
        return;
    }

    //Set up the items.
    for (unsigned int i = 0; i < _items.size(); ++i)
    {
        items.insert(items.end(), _items[i]);
        itemElements.insert(itemElements.end(),
                            GUILabel(params, textRender, TextRenderer::FontSlot(fontID, firstSlotIndex + i),
                                     itemTextMat, timeLerpSpeed, textOffset, GUILabel::VO_CENTER));
        if (!(itemElements.end() - 1)->SetText(items[i]))
        {
            BoxElement.Mat = 0;
            return;
        }
    }
}

void GUISelectionBox::SetTextColor(Vector4f newCol)
{
    textColor = newCol;
    for (unsigned int i = 0; i < items.size(); ++i)
        itemElements[i].Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(newCol);
}

bool GUISelectionBox::SetItem(unsigned int index, const std::string & newVal)
{
    if (index >= items.size()) return false;

    return itemElements[index].SetText(newVal);
}

Vector2f GUISelectionBox::GetCollisionDimensions(void) const
{
    return scale.ComponentProduct(ToV2f(Vector2u(BoxElement.Tex->GetWidth(), BoxElement.Tex->GetHeight())));
}

void GUISelectionBox::ScaleBy(Vector2f scaleAmount)
{
    scale.MultiplyComponents(scaleAmount);
    itemBackground.ScaleBy(scaleAmount);

    for (unsigned int i = 0; i < items.size(); ++i)
    {
        Vector2f itemCenter = itemElements[i].GetCollisionCenter();
        itemElements[i].SetPosition(itemCenter.ComponentProduct(scaleAmount));
        itemElements[i].ScaleBy(scaleAmount);
    }
}
void GUISelectionBox::SetScale(Vector2f newScale)
{
    ScaleBy(Vector2f(newScale.x / scale.x, newScale.y / scale.y));
}

std::string GUISelectionBox::Render(float elapsedTime, const RenderInfo & info)
{
    Vector4f myCol = GetColor();
    Vector4f oldCol;
    Vector2f boxSize = scale.ComponentProduct(Vector2f((float)BoxElement.Tex->GetWidth(),
                                                       (float)BoxElement.Tex->GetHeight())),
             halfBoxSize = boxSize * 0.5f;

    //Render the box.
    BoxElement.SetPosition(center);
    BoxElement.Depth = Depth;
    oldCol = BoxElement.GetColor();
    BoxElement.SetColor(myCol.ComponentProduct(oldCol));
    std::string err = BoxElement.Render(elapsedTime, info);
    BoxElement.SetPosition(Vector2f());
    BoxElement.SetColor(oldCol);
    if (!err.empty()) return "Error rendering main box: " + err;


    //Render the selected item.

    switch (itemElements[selectedItem].OffsetHorz)
    {
        case GUILabel::HO_LEFT:
            itemElements[selectedItem].SetPosition(Vector2f(-(boxSize.x * 0.5f), 0.0f));
            break;
        case GUILabel::HO_CENTER:
            itemElements[selectedItem].SetPosition(Vector2f());
            break;
        case GUILabel::HO_RIGHT:
            itemElements[selectedItem].SetPosition(Vector2f(boxSize.x * 0.5f, 0.0f));
            break;
        default:
            assert(false);
            return "Unknown horizontal offset enum value '" +
                        std::to_string(itemElements[selectedItem].OffsetHorz) + "'";
    }

    itemElements[selectedItem].MoveElement(center);
    itemElements[selectedItem].Depth = Depth + 0.0001f;
    oldCol = itemElements[selectedItem].GetColor();
    itemElements[selectedItem].SetColor(myCol.ComponentProduct(oldCol));
    err = itemElements[selectedItem].Render(elapsedTime, info);
    itemElements[selectedItem].MoveElement(-center);
    itemElements[selectedItem].SetColor(oldCol);
    if (!err.empty())
        return std::string() + "Error rendering selectd item '" + items[selectedItem] + "': " + err;

    //Render the other items and their background tex if this element is currently selected.
    if (IsExtended)
    {
        //Get the number of items to be rendered.
        unsigned int numbRenderedItems = 0;
        if (DrawEmptyItems)
        {
            numbRenderedItems = items.size();
        }
        else
        {
            for (unsigned int i = 0; i < items.size(); ++i)
                if (!items[i].empty())
                    ++numbRenderedItems;
        }

        //Render the item backdrop.
        
        //First calculate the bounds of the backdrop.
        Vector2f minBack(-halfBoxSize.x, 0.0f),
                 maxBack(halfBoxSize.x, 0.0f);
        if (ExtendAbove)
        {
            minBack.y = (float)(numbRenderedItems * boxSize.y) - halfBoxSize.y;
            maxBack.y = halfBoxSize.y;
        }
        else
        {
            minBack.y = -halfBoxSize.y;
            maxBack.y = (float)(numbRenderedItems * boxSize.y) + halfBoxSize.y;
        }
        itemBackground.SetBounds(minBack, maxBack);

        //Now render it.
        itemBackground.MoveElement(center);
        itemBackground.Depth = Depth + 0.00005f;
        oldCol = itemBackground.GetColor();
        itemBackground.SetColor(oldCol.ComponentProduct(myCol));
        err = itemBackground.Render(elapsedTime, info);
        itemBackground.MoveElement(-center);
        itemBackground.SetColor(oldCol);
        if (!err.empty())
            return std::string() + "Error rendering item list background: '" + err;


        //Render each text item (the currently-selected item was already displayed in the selection box).
        unsigned int itemIndex = 1;
        float dir = (ExtendAbove ? 1.0f : -1.0f);
        for (unsigned int i = 0; i < items.size(); ++i)
        {
            if (i == selectedItem || (!DrawEmptyItems && items[i].empty()))
                continue;

            Vector2f itemPos(0.0f, (float)boxSize.y * dir * (float)itemIndex);
            switch (itemElements[i].OffsetHorz)
            {
                case GUILabel::HO_LEFT:
                    itemPos.x = boxSize.x * -0.5f;
                    break;
                case GUILabel::HO_CENTER:
                    itemPos.x = 0.0f;
                    break;
                case GUILabel::HO_RIGHT:
                    itemPos.x = boxSize.x * 0.5f;
                    break;
                default:
                    assert(false);
                    return "Unknown horizontal offset enum value '" +
                        std::to_string(itemElements[i].OffsetHorz) + "'";
            }
            itemElements[i].SetPosition(itemPos);

            itemElements[i].MoveElement(center);
            itemElements[i].Depth = Depth + 0.00010f;
            oldCol = itemElements[i].GetColor();
            itemElements[i].SetColor(oldCol.ComponentProduct(myCol));
            err = itemElements[i].Render(elapsedTime, info);
            itemElements[i].MoveElement(-center);
            itemElements[i].SetColor(oldCol);
            if (!err.empty())
                return std::string() + "Error rendering item '" + items[i] + "': " + err;

            ++itemIndex;
        }
    }

    return "";
}

void GUISelectionBox::OnMouseClick(Vector2f relativeMousePos)
{
    bool inside = IsLocalInsideBounds(relativeMousePos);

    if (IsExtended)
    {
        //Check to see if any text box options were clicked.
        IsExtended = false;
        if (!inside)
        {
            for (unsigned int i = 0; i < items.size(); ++i)
            {
                if (i == selectedItem || (!DrawEmptyItems && items[i].empty()))
                    continue;

                if (itemElements[i].IsLocalInsideBounds(relativeMousePos - itemElements[i].GetCollisionCenter()))
                {
                    selectedItem = i;
                    return;
                }
            }
        }
    }
    else if (inside)
    {
        IsExtended = true;
    }
}
void GUISelectionBox::OnMouseDrag(Vector2f oldRelativeMousePos, Vector2f currentRelativeMousePos)
{
    Vector2f cent = itemBackground.GetCollisionCenter();
    itemBackground.OnMouseDrag(oldRelativeMousePos - cent, currentRelativeMousePos - cent);

    for (unsigned int i = 0; i < items.size(); ++i)
    {
        cent = itemElements[i].GetCollisionCenter();
        itemElements[i].OnMouseDrag(oldRelativeMousePos - cent, currentRelativeMousePos - cent);
    }
}
void GUISelectionBox::OnMouseRelease(Vector2f relativeMousePos)
{
    itemBackground.OnMouseRelease(relativeMousePos - itemBackground.GetCollisionCenter());
    for (unsigned int i = 0; i < items.size(); ++i)
        itemElements[i].OnMouseRelease(relativeMousePos - itemElements[i].GetCollisionCenter());
}

void GUISelectionBox::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    //TODO: Once highlight is set up for this class, calculate any mouse-overs. Don't just check whether each item is moused over; manually check the mouse position, because the selection hitbox shouldn't be limited to the text bounds.

    itemBackground.Update(elapsed, relativeMousePos - itemBackground.GetCollisionCenter());
    for (unsigned int i = 0; i < items.size(); ++i)
        itemElements[i].Update(elapsed, relativeMousePos - itemElements[i].GetCollisionCenter());
}