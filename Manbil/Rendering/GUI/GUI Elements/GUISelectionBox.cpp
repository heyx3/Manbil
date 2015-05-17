#include "GUISelectionBox.h"

#include "../GUIMaterials.h"


typedef GUISelectionBox GSB;

GSB::GUISelectionBox(TextRenderer* textRenderer, FreeTypeHandler::FontID font, Vector4f textColor,
                     bool mipmappedText, FilteringTypes textFilterQuality,
                     Vector2f _textScale, float _textSpacing,
                     Material* textRenderMat, const UniformDictionary& textRenderParams,
                     const GUITexture& mainBackground, const GUITexture& highlight,
                     const GUITexture& selectionBackground,
                     bool _extendAbove, std::string& outError,
                     const std::vector<std::string>& _items,
                     void(*onOptionSelected)(GUISelectionBox* selector, const std::string& item,
                                             unsigned int itemIndex, void* pData),
                     void(*onDropdownToggled)(GUISelectionBox* selector, void* pData),
                     unsigned int textRenderHeight,
                     void* onOptionSelected_pData, void* onDropdownToggled_pData,
                     float textAnimSpeed)
    : TextRender(textRenderer), MainBox(mainBackground), SelectionBackground(selectionBackground),
      Highlight(highlight), extendAbove(_extendAbove), isExtended(false), TextColor(textColor),
      FontID(font), itemFontID(font), textScale(_textScale), textSpacing(_textSpacing),
      OnOptionSelected(onOptionSelected), OnOptionSelected_pData(onOptionSelected_pData),
      OnDropdownToggled(onDropdownToggled), OnDropdownToggled_pData(onDropdownToggled_pData),
      GUIElement(UniformDictionary())
{
    //Give this selection box the time lerp param so that its color is animated.
    Params.Floats[GUIMaterials::DynamicQuadDraw_TimeLerp] =
        UniformValueF(0.0f, GUIMaterials::DynamicQuadDraw_TimeLerp);
    
    if (textRenderHeight == 0)
    {
        textRenderHeight = TextRender->GetMaxCharacterSize(FontID).y;
    }


    constructorData.textSettings = TextureSampleSettings2D(textFilterQuality, WT_CLAMP);
    constructorData.useMips = mipmappedText;
    constructorData.renderHeight = textRenderHeight;
    constructorData.labelRenderParams = textRenderParams;
    constructorData.labelRenderMat = textRenderMat;
    constructorData.textAnimSpeed = textAnimSpeed;

    ResetItems(_items, outError);
}
GSB::~GUISelectionBox(void)
{
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        bool tryD = TextRender->DeleteTextRenderSlot(itemElements[i].GetTextRenderSlot());
        assert(tryD);
    }
}

Box2D GSB::GetBounds(void) const
{
    if (isExtended)
    {
        Box2D bnds = SelectionBackground.GetBounds();
        bnds.Move(SelectionBackground.GetPos());
        return bnds;
    }
    else
    {
        Box2D bnds = MainBox.GetBounds();
        bnds.Move(MainBox.GetPos());
        return bnds;
    }
}
bool GSB::GetDidBoundsChangeDeep(void) const
{
    return DidBoundsChange ||
           (isExtended && MainBox.DidBoundsChange) ||
           (!isExtended && SelectionBackground.DidBoundsChange);
}
void GSB::ClearDidBoundsChangeDeep(void)
{
    DidBoundsChange = false;
    MainBox.DidBoundsChange = false;
    SelectionBackground.DidBoundsChange = false;
}

bool GSB::GetIsItemHidden(unsigned int i) const
{
    assert(i < itemsHidden.size());
    return itemsHidden[i] || (!drawEmptyItems && items[i].empty());
}
void GSB::SetIsItemHidden(unsigned int i, bool hide)
{
    bool wasAlreadyHidden = GetIsItemHidden(i);
    itemsHidden[i] = hide;
    if (hide && !wasAlreadyHidden)
    {
        nVisibleItems += 1;
    }
    else if (!hide && wasAlreadyHidden && (drawEmptyItems || !items[i].empty()))
    {
        nVisibleItems -= 1;
    }
}

void GSB::SetSelectedObject(unsigned int newIndex, bool raiseEvent)
{
    currentItem = newIndex;
    if (raiseEvent && OnOptionSelected != 0)
    {
        OnOptionSelected(this, items[currentItem], currentItem, OnOptionSelected_pData);
    }
}
void GSB::SetScale(Vector2f newScale)
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

    for (unsigned int i = 0; i < itemElements.size(); ++i)
    {
        itemElements[i].ScaleBy(delta);
    }
}
void GSB::SetDrawEmptyItems(bool shouldDraw)
{
    DidBoundsChange = true;
    drawEmptyItems = shouldDraw;

    nVisibleItems = 0;
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        if (GetIsItemHidden(i))
        {
            nVisibleItems += 1;
        }
    }
}

std::string GSB::SetItem(unsigned int index, std::string newVal)
{
    bool hiddenByList = itemsHidden[index],
         hiddenByEmpty = items[index].empty();
    std::string oldVal = items[index];

    if (!itemElements[index].SetText(newVal))
    {
        return "";
    }

    items[index] = newVal;

    //See if this text change just added/removed the element from being rendered.
    if (!drawEmptyItems && !hiddenByList)
    {
        //If the text went from empty to non-empty, we gained a visible element.
        if (hiddenByEmpty && !newVal.empty())
        {
            nVisibleItems += 1;
            assert(nVisibleItems < items.size());

            if (isExtended)
            {
                PositionSelectionBackground();
            }
        }
        //Otherwise, if the text went from non-empty to empty, we lost a visible element.
        else if (!hiddenByEmpty && newVal.empty())
        {
            assert(nVisibleItems > 0);
            nVisibleItems -= 1;
            if (nVisibleItems > 0)
            {
                while (items[currentItem].empty())
                {
                    currentItem = (currentItem + 1) % items.size();
                }
            }

            if (isExtended)
            {
                PositionSelectionBackground();
            }
        }
    }

    return oldVal;
}

void GSB::SetExtendsAbove(bool newVal)
{
    extendAbove = newVal;
    if (isExtended)
    {
        DidBoundsChange = true;
    }
}

void GSB::SetIsExtended(bool _isExtended, bool raiseEvent)
{
    DidBoundsChange = true;
    isExtended = _isExtended;

    if (isExtended)
    {
        PositionSelectionBackground();
    }

    if (raiseEvent && OnDropdownToggled != 0)
    {
        OnDropdownToggled(this, OnDropdownToggled_pData);
    }
}

void GSB::ResetItems(const std::vector<std::string>& newItems, std::string& err)
{
    //Clear out the current stuff.
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        bool tryD = TextRender->DeleteTextRenderSlot(itemElements[i].GetTextRenderSlot());
        assert(tryD);
    }
    items = newItems;
    itemElements.clear();
    itemElements.reserve(newItems.size());
    itemsHidden.resize(newItems.size());


    unsigned int textBackWidth = (unsigned int)MainBox.GetBounds().GetXSize() / textScale.x;
    nVisibleItems = 0;
    for (unsigned int i = 0; i < items.size(); ++i)
    {
        itemsHidden[i] = false;

        //Try to create the font slot.
        TextRenderer::FontSlot slot = TextRender->CreateTextRenderSlot(FontID, err,
                                                                       textBackWidth,
                                                                       constructorData.renderHeight,
                                                                       constructorData.useMips,
                                                                       constructorData.textSettings);
        if (!err.empty())
        {
            err = "Error creating text render slot for '" + items[i] + "': " + err;
            return;
        }

        //Create the label.
        itemElements.push_back(GUILabel(constructorData.labelRenderParams, TextRender, slot,
                                        constructorData.labelRenderMat, constructorData.textAnimSpeed,
                                        GUILabel::HO_LEFT, GUILabel::VO_CENTER));
        itemElements[i].Depth = 0.001f;
        itemElements[i].ScaleBy(textScale);

        //Set the label text.
        if (!itemElements[i].SetText(items[i]))
        {
            err = "Error rendering '" + items[i] + "' into a label: " + err;
            return;
        }

        //Calculate whether the element will be rendered in the dropdown box.
        if (drawEmptyItems || !items[i].empty())
        {
            nVisibleItems += 1;
        }
    }
}

void GSB::PositionSelectionBackground(void)
{
    if (isExtended)
    {
        DidBoundsChange = true;
    }


    //Compute the minY and height of the background.

    Box2D mainBoxBounds = MainBox.GetBounds();

    float minY = mainBoxBounds.GetYMin();
    if (extendAbove)
    {
        minY -= (nVisibleItems - 1) * (mainBoxBounds.GetYSize() + textSpacing);
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

    Box2D bnds = GetBounds();
}

void GSB::OnMouseClick(Vector2f relativeMousePos)
{
    if (!IsClickable)
    {
        return;
    }

    //If the dropdown box is open, see if the player clicked on an option.
    if (isExtended)
    {
        if (mousedOverItem >= 0)
        {
            //Select the item that is currently moused over.

            currentItem = (unsigned int)mousedOverItem;

            SetIsExtended(false, false);
            SetSelectedObject((unsigned int)mousedOverItem, true);
        }
        else
        {
            SetIsExtended(false, true);
        }
    }
    //Otherwise, see if the player clicked to open the dropdown box.
    else
    {
        if (MainBox.GetBounds().IsPointInside(relativeMousePos))
        {
            SetIsExtended(true);
        }
    }
}

void GSB::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    //Update standalone GUI elements.
    MainBox.Update(elapsed, relativeMousePos - MainBox.GetPos());
    SelectionBackground.Update(elapsed, relativeMousePos - SelectionBackground.GetPos());
    Highlight.Update(elapsed, relativeMousePos - Highlight.GetPos());


    //Update the item labels and track which ones are visible.
    //TODO: Store "visibleIndices" as a member field as a replacement for "nVisibleItems".
    std::vector<unsigned int> visibleIndices;
    for (unsigned int i = 0; i < itemElements.size(); ++i)
    {
        itemElements[i].Update(elapsed, relativeMousePos - itemElements[i].GetPos());
        if (!GetIsItemHidden(i))
        {
            visibleIndices.insert(visibleIndices.end(), i);
        }
    }


    //See if any items are moused over.
    if (isExtended || visibleIndices.size() > 0)
    {
        Box2D bounds = SelectionBackground.GetBounds();
        bounds.Move(SelectionBackground.GetPos());

        if (bounds.PointTouches(relativeMousePos))
        {
            //Find the closest entry.
            
            Box2D mainBoxBounds = MainBox.GetBounds(),
                  backgroundBounds = SelectionBackground.GetBounds();
            float ySpace = (extendAbove ? -textSpacing : textSpacing);
            float yIncrement = (extendAbove ? -mainBoxBounds.GetYSize() : mainBoxBounds.GetYSize()) + ySpace;

            //TODO: Figure out where this is supposed to be used.
            const float moveDir = (extendAbove ? -0.5f : 0.5f);

            unsigned int closestElement = 0;
            float closestDist = Mathf::Abs(relativeMousePos.y);

            for (unsigned int i = 1; i < visibleIndices.size(); ++i)
            {
                float yOffset = yIncrement * (float)i;

                float tempDist = Mathf::Abs(yOffset - relativeMousePos.y);
                if (tempDist < closestDist)
                {
                    closestDist = tempDist;
                    closestElement = i;
                }
            }

            mousedOverItem = visibleIndices[closestElement];
        }
        else
        {
            mousedOverItem = -1;
        }
    }
    else
    {
        mousedOverItem = -1;
    }
}

void GSB::ScaleBy(Vector2f scaleAmount)
{
    SetScale(GetScale().ComponentProduct(scaleAmount));
}

void GSB::Render(float elapsedTime, const RenderInfo& info)
{
    std::string err;

    if (!isExtended)
    {
        if (MainBox.IsValid())
        {
            RenderChild(&MainBox, elapsedTime, info);
        }

        itemElements[currentItem].SetColor(TextColor);
        itemElements[currentItem].SetPosition(Vector2f(MainBox.GetBounds().GetXMin(), 0.0f));
        itemElements[currentItem].Depth = 0.0005f;
        RenderChild(&itemElements[currentItem], elapsedTime, info);
    }
    else
    {
        float ySpace = (extendAbove ? -textSpacing : textSpacing);
        Box2D mainBoxBounds = MainBox.GetBounds(),
              backgroundBounds = SelectionBackground.GetBounds();


        //Calculate the position/size of the background and draw it.
        if (SelectionBackground.IsValid())
        {
            SelectionBackground.Depth = -0.001f;
            RenderChild(&SelectionBackground, elapsedTime, info);
        }

        //Draw all the visible items.
        unsigned int visibleItemIndex = 0;
        float yIncrement = (extendAbove ? -mainBoxBounds.GetYSize() : mainBoxBounds.GetYSize()) + ySpace;
        for (unsigned int i = 0; i < itemElements.size(); ++i)
        {
            if (GetIsItemHidden(i))
            {
                continue;
            }

            Box2D bounds = itemElements[i].GetBounds();

            float yOffset = yIncrement * (float)visibleItemIndex;
            itemElements[i].SetPosition(Vector2f(backgroundBounds.GetXMin(), yOffset));

            visibleItemIndex += 1;

            itemElements[i].SetColor(TextColor);
            itemElements[i].Depth = 0.001f * (float)i;

            RenderChild(&itemElements[i], elapsedTime, info);
        }

        //Draw the highlight over the moused-over object.
        if (mousedOverItem >= 0 && Highlight.IsValid() &&
            !GetIsItemHidden(mousedOverItem))
        {
            //Get which item in the menu it is.
            unsigned int menuIndex = (int)mousedOverItem;
            unsigned int visibleIndex = 0;
            for (unsigned int i = 0; i < items.size(); ++i)
            {
                if (i == (unsigned int)mousedOverItem)
                {
                    menuIndex = visibleIndex;
                    break;
                }
                if (!GetIsItemHidden(i))
                {
                    visibleIndex += 1;
                }
            }

            //Get the Y value of the highlighted item.
            Highlight.SetBounds(Box2D(Vector2f(mainBoxBounds.GetCenterX(),
                                               yIncrement * (float)menuIndex),
                                      Highlight.GetBounds().GetDimensions()));
            Highlight.Depth = 0.02f;

            RenderChild(&Highlight, elapsedTime, info);
        }
    }
}