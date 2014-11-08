#include "GUISelectionBox.h"



GUISelectionBox::GUISelectionBox(std::string& outError, TextRenderer* textRenderer,
                                 const GUITexture& mainBox, const GUITexture& highlight,
                                 const GUITexture& selectionBackground, bool _extendAbove,
                                 Vector4f textColor, FreeTypeHandler::FontID font,
                                 Material* textRenderMat, const UniformDictionary& textRenderParams,
                                 bool mipmappedText, FilteringTypes textFilterQuality,
                                 const std::vector<std::string>& _items, Vector2f tScale, float tSpacing,
                                 void(*onOptionSelected)(GUISelectionBox* selector, const std::string& item,
                                                         unsigned int itemIndex, void* pData),
                                 void(*onDropdownToggled)(GUISelectionBox* selector, void* pData),
                                 void* onOptionSelected_pData, void* onDropdownToggled_pData,
                                 float textAnimSpeed)
    : TextRender(textRenderer), MainBox(mainBox), SelectionBackground(selectionBackground),
      Highlight(highlight), extendAbove(_extendAbove), isExtended(false), TextColor(textColor),
      FontID(font), items(_items), itemFontID(font), textScale(tScale), textSpacing(tSpacing),
      OnOptionSelected(onOptionSelected), OnOptionSelected_pData(onOptionSelected_pData),
      OnDropdownToggled(onDropdownToggled), OnDropdownToggled_pData(onDropdownToggled_pData),
      GUIElement(UniformDictionary())
{
    //Create one render slot for each item.
    unsigned int firstSlotIndex = TextRender->GetNumbSlots(font);
    bool tryCreate = TextRender->CreateTextRenderSlots(FontID, (unsigned int)mainBox.GetBounds().GetXSize(),
                                                       TextRender->GetMaxCharacterSize(FontID).y,
                                                       mipmappedText,
                                                       TextureSampleSettings2D(textFilterQuality, WT_CLAMP),
                                                       items.size());
    if (!tryCreate)
    {
        outError = "Error creating " + std::to_string(items.size()) +
                       " text render slots: " + TextRender->GetError();
        return;
    }


    //Set up the items.
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        itemElements.insert(itemElements.end(),
                            GUILabel(textRenderParams, TextRender,
                                     TextRenderer::FontSlot(FontID, firstSlotIndex + i),
                                     textRenderMat, textAnimSpeed,
                                     GUILabel::HO_LEFT, GUILabel::VO_CENTER));
        itemElements[items.size() - 1].Depth = Depth + 0.0001f;
        itemElements[items.size() - 1].ScaleBy(textScale);

        if (!itemElements[itemElements.size() - 1].SetText(items[i]))
        {
            outError = "Error rendering '" + items[i] + "' into a label: " + TextRender->GetError();
            return;
        }
    }
}

Box2D GUISelectionBox::GetBounds(void) const
{
    if (isExtended)
    {
        return SelectionBackground.GetBounds();
    }
    else
    {
        return MainBox.GetBounds();
    }
}
bool GUISelectionBox::GetDidBoundsChangeDeep(void) const
{
    return DidBoundsChange ||
           (isExtended && MainBox.DidBoundsChange) ||
           (!isExtended && SelectionBackground.DidBoundsChange);
}
void GUISelectionBox::ClearDidBoundsChangeDeep(void)
{
    DidBoundsChange = false;
    MainBox.DidBoundsChange = false;
    SelectionBackground.DidBoundsChange = false;
}

void GUISelectionBox::SetSelectedObject(unsigned int newIndex, bool raiseEvent)
{
    currentItem = newIndex;
    if (raiseEvent && OnOptionSelected != 0)
        OnOptionSelected(this, items[currentItem], currentItem, OnOptionSelected_pData);
}
void GUISelectionBox::SetScale(Vector2f newScale)
{
    Vector2f oldScale = GetScale();
    GUIElement::SetScale(newScale);

    Vector2f delta(newScale.x / oldScale.x, newScale.y / oldScale.y);
    MainBox.ScaleBy(delta);
    MainBox.SetPosition(MainBox.GetPos().ComponentProduct(delta));
    SelectionBackground.ScaleBy(delta);
    SelectionBackground.SetPosition(SelectionBackground.GetPos().ComponentProduct(delta));
    Highlight.ScaleBy(delta);
    Highlight.SetPosition(Highlight.GetPos().ComponentProduct(delta));
}

std::string GUISelectionBox::SetItem(unsigned int index, std::string newVal)
{
    std::string oldVal = items[index];
    items[index] = newVal;

    //If one value is empty and the other not empty, and empty items aren't drawn,
    //    then the dropdown background just changed.
    if (isExtended && !drawEmptyItems && (oldVal.empty() != newVal.empty()))
    {
        PositionSelectionBackground();
    }

    return oldVal;
}

void GUISelectionBox::SetIsExtended(bool _isExtended, bool raiseEvent)
{
    DidBoundsChange = true;
    isExtended = _isExtended;

    if (isExtended) PositionSelectionBackground();

    if (raiseEvent && OnDropdownToggled != 0)
    {
        OnDropdownToggled(this, OnDropdownToggled_pData);
    }
}
void GUISelectionBox::PositionSelectionBackground(void)
{
    if (isExtended) DidBoundsChange = true;

    //Count the number of visible items.
    unsigned int nVisibleItems;
    if (drawEmptyItems)
    {
        nVisibleItems = items.size();
    }
    else
    {
        nVisibleItems = 0;
        for (unsigned int i = 0; i < items.size(); ++i)
        {
            if (!items[i].empty())
                nVisibleItems += 1;
        }
    }


    //Compute the minimum Y value and height of the background.

    Box2D mainBoxBounds = MainBox.GetBounds();

    float minY = mainBoxBounds.GetCenterY() - (0.5f * mainBoxBounds.GetYSize());
    if (extendAbove)
    {
        minY -= (nVisibleItems - 1) * (mainBoxBounds.GetYSize() * textSpacing);
    }

    float nVisibleItemsF = (float)nVisibleItems;
    float height = (nVisibleItemsF * mainBoxBounds.GetYSize()) +
                   ((nVisibleItemsF - 1) * textSpacing);
    Box2D backBounds(mainBoxBounds.GetXMin(), minY,
                     Vector2f(mainBoxBounds.GetXSize(), height));


    //Finally, set the background bounds. This GUISelectionBox is already marked as changed,
    //    so don't change the background's "changed" flag.
    bool oldVal = SelectionBackground.DidBoundsChange;
    SelectionBackground.SetBounds(backBounds);
    SelectionBackground.DidBoundsChange = oldVal;
}

void GUISelectionBox::OnMouseClick(Vector2f relativeMousePos)
{
    if (!IsClickable) return;

    if (isExtended)
    {
        Box2D bounds = SelectionBackground.GetBounds();

        SetIsExtended(false);

        if (bounds.IsPointInside(relativeMousePos))
        {
            assert(mousedOverItem >= 0);
            currentItem = (unsigned int)mousedOverItem;

            SetIsExtended(false, false);

            if (OnOptionSelected != 0)
            {
                OnOptionSelected(this, items[currentItem], currentItem, OnOptionSelected_pData);
            }
        }
        else
        {
            SetIsExtended(false);
        }
    }
    else
    {
        Box2D bounds = MainBox.GetBounds();

        if (bounds.IsPointInside(relativeMousePos))
        {
            SetIsExtended(true);
        }
    }
}

void GUISelectionBox::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    MainBox.Update(elapsed, relativeMousePos);
    SelectionBackground.Update(elapsed, relativeMousePos);
    Highlight.Update(elapsed, relativeMousePos);
    for (unsigned int i = 0; i < itemElements.size(); ++i)
        itemElements[i].Update(elapsed, relativeMousePos);

    //TODO: See if user is mousing over any items.
}
std::string GUISelectionBox::Render(float elapsedTime, const RenderInfo& info)
{
    std::string err;


    if (!isExtended)
    {
        if (MainBox.IsValid())
        {
            err = RenderChild(&MainBox, elapsedTime, info);
            if (!err.empty())
            {
                return "Error rendering main box: " + err;
            }
        }

        err = itemElements[currentItem].Render(elapsedTime, info);
        if (!err.empty())
        {
            return "Error rendering selected text: " + err;
        }
    }
    else
    {
        float ySpace = (extendAbove ? -textSpacing : textSpacing);
        Box2D mainBoxBounds = MainBox.GetBounds(),
              backgroundBounds = SelectionBackground.GetBounds();

        float backgroundBaseHeight = (extendAbove ?
                                        (backgroundBounds.GetYMax() - (mainBoxBounds.GetYSize() * 0.5f)) :
                                        (backgroundBounds.GetYMin() + (mainBoxBounds.GetYSize() * 0.5f)));


        //Draw all the items.
        unsigned int nVisibleItems = 0;
        for (unsigned int i = 0; i < itemElements.size(); ++i)
        {
            if (!drawEmptyItems && items[i].empty())
                continue;

            Box2D bounds = itemElements[i].GetBounds();

            float yOffset = (mainBoxBounds.GetYSize() + ySpace) * (float)nVisibleItems;
            itemElements[i].SetPosition(Vector2f(backgroundBounds.GetXMin(),
                                                 backgroundBaseHeight + yOffset));

            nVisibleItems += 1;

            err = RenderChild(&itemElements[i], elapsedTime, info);
            if (!err.empty())
            {
                return "Error rendering text item '" + items[i] + "': " + err;
            }
        }

        //Calculate the position/size of the background and draw it.
        if (SelectionBackground.IsValid())
        {
            err = RenderChild(&SelectionBackground, elapsedTime, info);
            if (!err.empty())
            {
                return "Error rendering selection background: " + err;
            }
        }

        //Draw the highlight over the moused-over object.
        if (mousedOverItem >= 0 && Highlight.IsValid() &&
            (drawEmptyItems || !items[mousedOverItem].empty()))
        {
            //Get which item in the menu it is.
            unsigned int menuIndex = (int)mousedOverItem;
            if (!drawEmptyItems)
            {
                unsigned int nVisible = 0;
                for (unsigned int i = 0; i < items.size(); ++i)
                {
                    if (!items[i].empty())
                    {
                        nVisible += 1;
                        if (nVisible == mousedOverItem + 1)
                        {
                            menuIndex = i;
                            break;
                        }
                    }
                }
            }

            //Get the Y value of the highlighted item.
            Highlight.SetBounds(Box2D(Vector2f(mainBoxBounds.GetCenterX(),
                                               backgroundBaseHeight +
                                                   ((mainBoxBounds.GetYSize() + ySpace) *
                                                       (float)menuIndex)),
                                      Highlight.GetBounds().GetDimensions()));

            err = RenderChild(&Highlight, elapsedTime, info);
            if (!err.empty())
            {
                return "Error rendering highlight: " + err;
            }
        }
    }

    return "";
}