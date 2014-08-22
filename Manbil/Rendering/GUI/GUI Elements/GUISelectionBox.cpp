#include "GUISelectionBox.h"


GUISelectionBox::GUISelectionBox(TextRenderer* textRender, Material* selectionBoxMat,
                                 MTexture2D * selectionBoxTex,
                                 unsigned int fontID, Vector2u fontRenderTexSize,
                                 const TextureSampleSettings2D & fontRenderSettings,
                                 Material * itemTextMat,
                                 const GUITexture & itemListBackground, 
                                 std::vector<std::string> _items, unsigned int selected,
                                 bool extendAbove, float timeLerpSpeed)
    : GUIElement(timeLerpSpeed), TextRender(textRender), BoxMat(selectionBoxMat), BoxTex(selectionBoxTex),
      items(_items), ExtendAbove(extendAbove), selectedItem(selected),
      itemBackground(itemListBackground), itemFontID(fontID), IsExtended(false)
{
    unsigned int firstSlotIndex = TextRender->GetNumbSlots(fontID);
    bool tryCre = TextRender->CreateTextRenderSlots(fontID, fontRenderTexSize.x, fontRenderTexSize.y, false,
                                                    fontRenderSettings, items.size());
    if (tryCre)
    {
        assert(BoxMat != 0);
        BoxMat = 0;
        return;
    }

    //Set up the items.
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        itemElements.insert(itemElements.end(),
                            GUILabel(textRender, TextRenderer::FontSlot(fontID, firstSlotIndex + i),
                                     itemTextMat, timeLerpSpeed, GUILabel::HO_LEFT, GUILabel::VO_CENTER));
        if (!itemElements.end()->SetText(items[i]))
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
    return ToV2f(Vector2u(BoxTex->GetWidth(), BoxTex->GetHeight())).ComponentProduct(scale).RoundToInt();
}

void GUISelectionBox::ScaleBy(Vector2f scaleAmount)
{
    scale.MultiplyComponents(scaleAmount);
    itemBackground.ScaleBy(scaleAmount);

    for (unsigned int i = 0; i < items.size(); ++i)
    {
        Vector2f itemCenter = itemElements[i].GetCollisionCenter();
        itemElements[i].SetPosition(itemCenter.ComponentProduct(scaleAmount).RoundToInt());
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
    Vector2f dimensions = GetCollisionDimensions();
    SetUpQuad(info, center, Depth, scale.ComponentProduct(dimensions));
    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture =
        TextRender->GetRenderedString(itemElements[selectedItem].TextRenderSlot)->GetTextureHandle();
    if (!GetQuad()->Render(info, Params, *BoxMat))
        return "Error rendering selection box texture: " + BoxMat->GetErrorMsg();


    //Render the selected item.

    Vector2u boxSize(itemBackground.Tex->GetWidth(), BoxTex->GetHeight());

    itemElements[selectedItem].SetPosition(Vector2f((float)boxSize.x, 0.0f));

    Vector2f delta(center.x +
                    BasicMath::RoundToInt((float)itemElements[selectedItem].GetCollisionCenter().x *
                                           scale.x),
                   0);
    itemElements[selectedItem].MoveElement(delta);
    std::string err = itemElements[selectedItem].Render(elapsedTime, info);
    itemElements[selectedItem].MoveElement(-delta);
    if (!err.empty()) return "Error rendering selectd item '" + items[selectedItem] + "': " + err;


    if (IsExtended)
    {
        int dir = (ExtendAbove ? -1 : 1);


        //Render the item backdrop.

        //TODO: Scale the item backdrop's height based on the number of items to render (keep in mind that the selected item is displayed in the selection box).
        Vector2i backPos((int)itemBackground.Tex->GetWidth() - (int)BoxTex->GetWidth(),
                         dir * ((int)BoxTex->GetHeight() + (itemBackground.Tex->GetHeight() / 2)));
        itemBackground.SetPosition(ToV2f(backPos).ComponentProduct(scale));

        Vector2f childPos = itemBackground.GetCollisionCenter();
        Vector2f childElementDelta = center + childPos.ComponentProduct(scale);
                   
        itemBackground.MoveElement(childElementDelta);
        itemBackground.Depth = Depth + 0.001f;
        err = itemBackground.Render(elapsedTime, info);
        if (!err.empty()) return "Error rendering selection box item backdrop: " + err;


        //Render each text item (the currently-selected item was already displayed in the selection box).
        unsigned int numbItems = 1;
        for (unsigned int i = 0; i < items.size(); ++i)
        {
            if (i == selectedItem || (!DrawEmptyItems && items[i].size() == 0))
                continue;

            itemElements[i].SetPosition(Vector2f(-(float)boxSize.x, (float)boxSize.y * (float)dir * (float)numbItems));

            //TODO; This was supposed to be either a "+" or a "-", but it was "=" as a typo. I went with plus sign; figure out what it really is.
            delta = center + itemElements[i].GetCollisionCenter().ComponentProduct(scale);
            itemElements[i].MoveElement(delta);

            itemElements[i].Depth = Depth + 0.0015f;
            err = itemElements[i].Render(elapsedTime, info);
            if (!err.empty()) return "Error rendering item '" + items[i] + "': " + err;

            itemElements[i].MoveElement(-delta);

            ++numbItems;
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
            int dir = (ExtendAbove ? -1 : 1);

            Vector2u boxSize(itemBackground.Tex->GetWidth(), BoxTex->GetHeight());
            unsigned int numbItems = 1;
            for (unsigned int i = 0; i < items.size(); ++i)
            {
                if (i == selectedItem || (!DrawEmptyItems && items[i].size() == 0))
                    continue;

                Vector2f pos(-(float)boxSize.x, (float)boxSize.y * dir * (float)numbItems);
                itemElements[i].SetPosition(pos);

                ++numbItems;

                if (itemElements[i].IsLocalInsideBounds(relativeMousePos - pos))
                {
                    selectedItem = i;
                    break;
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
    //TODO: Once highlight is set up for this class, calculate any mouse-overs.

    itemBackground.Update(elapsed, relativeMousePos - itemBackground.GetCollisionCenter());
    for (unsigned int i = 0; i < items.size(); ++i)
        itemElements[i].Update(elapsed, relativeMousePos - itemElements[i].GetCollisionCenter());
}