#include "GUISelectionBox.h"


GUISelectionBox::GUISelectionBox(const UniformDictionary & params,
                                 TextRenderer* textRender, Material* selectionBoxMat,
                                 MTexture2D * selectionBoxTex,
                                 unsigned int fontID, Vector2u fontRenderTexSize,
                                 const TextureSampleSettings2D & fontRenderSettings,
                                 Material * itemTextMat, GUILabel::HorizontalOffsets textOffset,
                                 const GUITexture & itemListBackground, 
                                 const std::vector<std::string> & _items, unsigned int selected,
                                 bool extendAbove, float timeLerpSpeed)
    : GUIElement(params, timeLerpSpeed), TextRender(textRender),
      BoxMat(selectionBoxMat), BoxTex(selectionBoxTex),
      ExtendAbove(extendAbove), selectedItem(selected),
      scale(1.0f, 1.0f),
      itemBackground(itemListBackground), itemFontID(fontID), IsExtended(false)
{
    unsigned int firstSlotIndex = TextRender->GetNumbSlots(fontID);
    bool tryCre = TextRender->CreateTextRenderSlots(fontID, fontRenderTexSize.x, fontRenderTexSize.y, false,
                                                    fontRenderSettings, _items.size());
    if (!tryCre)
    {
        assert(BoxMat != 0);
        BoxMat = 0;
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
            assert(BoxMat != 0);
            BoxMat = 0;
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
    return ToV2f(Vector2u(BoxTex->GetWidth(), BoxTex->GetHeight())).ComponentProduct(scale);
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
    Vector2f delta(newScale.x / scale.x, newScale.y / scale.y);
    ScaleBy(delta);
}

std::string GUISelectionBox::Render(float elapsedTime, const RenderInfo & info)
{
    //Render the box.
    Vector2f dimensions = GetCollisionDimensions(),
             halfDims = dimensions * 0.5f;
    SetUpQuad(info, center, Depth, dimensions);
    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture = BoxTex->GetTextureHandle();
    if (!GetQuad()->Render(info, Params, *BoxMat))
        return "Error rendering selection box texture: " + BoxMat->GetErrorMsg();


    //Render the selected item.

    Vector2u boxSize(itemBackground.Tex->GetWidth(), BoxTex->GetHeight());

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
                        std::to_string(itemElements[selectedItem].OffsetHorz);
    }

    itemElements[selectedItem].MoveElement(center);
    itemElements[selectedItem].Depth = Depth + 0.0001f;
    std::string err = itemElements[selectedItem].Render(elapsedTime, info);
    itemElements[selectedItem].MoveElement(-center);
    if (!err.empty()) return "Error rendering selectd item '" + items[selectedItem] + "': " + err;


    if (IsExtended)
    {
        // Get the number of items to be rendered.
        unsigned int numbRenderedItems = 0;
        if (DrawEmptyItems)
            numbRenderedItems = items.size();
        else for (unsigned int i = 0; i < items.size(); ++i)
            if (!items[i].empty())
                ++numbRenderedItems;

        //Render the item backdrop.
        
        //First calculate the bounds of the backdrop.
        Vector2f minBack(-halfDims.x, 0.0f),
                 maxBack(halfDims.x, 0.0f);
        if (ExtendAbove)
        {
            minBack.y = (float)(numbRenderedItems * boxSize.y) - halfDims.y;
            maxBack.y = halfDims.y;
        }
        else
        {
            minBack.y = -halfDims.y;
            maxBack.y = (float)(numbRenderedItems * boxSize.y) + halfDims.y;
        }
        itemBackground.SetBounds(minBack, maxBack);

        //Now render it.
        itemBackground.MoveElement(center);
        itemBackground.Depth = Depth + 0.00005f;
        err = itemBackground.Render(elapsedTime, info);
        itemBackground.MoveElement(-center);
        if (!err.empty()) return "Error rendering item background: '" + err;


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
                        std::to_string(itemElements[i].OffsetHorz);
            }
            itemElements[i].SetPosition(itemPos);

            itemElements[i].MoveElement(center);
            itemElements[i].Depth = Depth + 0.0015f;
            err = itemElements[i].Render(elapsedTime, info);
            itemElements[i].MoveElement(-center);
            if (!err.empty()) return "Error rendering item '" + items[i] + "': " + err;

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