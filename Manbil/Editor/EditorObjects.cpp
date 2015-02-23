#include "EditorObjects.h"

#include "../Rendering/GUI/GUI Elements/GUIPanel.h"



CheckboxValue::CheckboxValue(EditorObject::DescriptionData description,
                             Vector2f offset, bool defaultVal)
    : DefaultValue(defaultVal), EditorObject(description, offset)
{

}

std::string CheckboxValue::InitGUIElement(EditorMaterialSet& materialSet)
{
   MTexture2D *boxTex = &materialSet.CheckBoxBackgroundTex,
              *checkTex = &materialSet.CheckBoxCheckTex;
   GUITexture boxGUITex(materialSet.GetAnimatedMatParams(boxTex), boxTex,
                        materialSet.GetAnimatedMaterial(boxTex), true, materialSet.AnimateSpeed),
              checkGUITex(materialSet.GetAnimatedMatParams(checkTex), checkTex,
                          materialSet.GetAnimatedMaterial(checkTex), false, materialSet.AnimateSpeed);
    GUICheckbox* box = new GUICheckbox(materialSet.StaticMatGreyParams, boxGUITex, checkGUITex);

    box->SetChecked(DefaultValue, false);
    box->OnClicked = OnBoxClicked;
    box->OnClicked_Data = OnBoxClicked_Data;

    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(box);
    }
    else
    {
        std::string err;
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(box), err);
        if (activeGUIElement.get() == 0)
        {
            return err;
        }
    }
    return "";
}

DropdownValues::DropdownValues(const std::vector<std::string>& items, Vector2f offset,
                               EditorObject::DescriptionData description,
                               void(*onSelected)(GUISelectionBox* dropdownBox, const std::string& item,
                                                 unsigned int index, void* pData),
                               void* onSelected_Data,
                               void(*onUpdate)(GUISelectionBox* dropdownBox, void* pData),
                               void* onUpdate_Data)
    : Items(items), OnUpdate(onUpdate), OnUpdate_Data(onUpdate_Data),
      OnSelected(onSelected), OnSelected_Data(onSelected_Data),
      EditorObject(description, offset)
{

}
std::string DropdownValues::InitGUIElement(EditorMaterialSet& materialSet)
{
    GUITexture itemListBackground(materialSet.GetStaticMatParams(&materialSet.SelectionBoxBackgroundTex),
                                  &materialSet.SelectionBoxBackgroundTex,
                                  materialSet.GetStaticMaterial(&materialSet.SelectionBoxBackgroundTex),
                                  false, materialSet.AnimateSpeed);
    GUITexture highlight;
    GUITexture selectedItemBox(materialSet.GetAnimatedMatParams(&materialSet.SelectionBoxBoxTex),
                               &materialSet.SelectionBoxBoxTex,
                               materialSet.GetAnimatedMaterial(&materialSet.SelectionBoxBoxTex),
                               false, materialSet.AnimateSpeed);
    //GUISelectionBox* box = new GUISelectionBox(err, &materialSet.TextRender,
    //                                           selectedItemBox, materialSet.FontID,
    //                                           Vector2u(selectedItemBox.Tex->GetWidth(),
    //                                           selectedItemBox.Tex->GetHeight()),
    //                                           TextureSampleSettings2D(FT_LINEAR, WT_CLAMP),
    //                                           materialSet.StaticMatText, GUILabel::HO_LEFT,
    //                                           itemListBackground, Items, 0, true, materialSet.AnimateSpeed);
    std::string err;
    GUISelectionBox* box = new GUISelectionBox(&materialSet.TextRender, materialSet.FontID,
                                               materialSet.TextColor, true, FilteringTypes::FT_LINEAR,
                                               materialSet.TextScale, materialSet.DropdownBoxItemSpacing,
                                               materialSet.StaticMatText,
                                               materialSet.StaticMatTextParams,
                                               selectedItemBox, highlight, itemListBackground,
                                               true, err,
                                               Items,
                                               OnSelected, 0,
                                               OnSelected_Data, 0, materialSet.AnimateSpeed);
    if (!err.empty())
    {
        delete box;
        activeGUIElement = GUIElementPtr(0);
        return "Error generating text slots for dropdown box: " + err;;
    }

    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(box);
    }
    else
    {
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(box), err);
        if (activeGUIElement.get() == 0)
        {
            return "Error adding description label: " + err;
        }
    }
    return "";
}

EditorButtonData::EditorButtonData(std::string text, MTexture2D* tex, Vector2f buttonScale,
                                   void(*onClick)(GUITexture* clicked, Vector2f localMouse, void* pData),
                                   void* onClick_pData)
    : Text(text), Tex(tex), ButtonScale(buttonScale),
      OnClick(onClick), OnClick_pData(onClick_pData)
{

}
EditorButton::EditorButton(std::string text, Vector2f size,
                           MTexture2D* buttonTex, Vector2f offset,
                           EditorObject::DescriptionData description,
                           void(*onClick)(GUITexture* clicked, Vector2f localMouse, void* pData),
                           void* onClick_Data)
    : Text(text), ButtonSize(size), TexToUse(buttonTex),
      OnClick(onClick), OnClick_Data(onClick_Data),
      EditorObject(description, offset)
{

}
std::string EditorButton::InitGUIElement(EditorMaterialSet& materialSet)
{
    GUIElementPtr buttonTex(0),
                  buttonLabel(0);

    if (!Text.empty())
    {
        //First try to create the font slot to render the label.
        std::string err;
        unsigned int finalRenderWidth = (unsigned int)(ButtonSize.x / materialSet.TextScale.x);
        if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID, err,
                                                          finalRenderWidth,
                                                          materialSet.TextRenderSpaceHeight,
                                                          false,
                                                          TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
        {
            activeGUIElement = GUIElementPtr(0);
            return "Error creating text render slot for button '" + Text + "'s label: " + err;
        }
        TextRenderer::FontSlot labelSlot(materialSet.FontID,
                                         materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);
        //Next try to render the text.
        if (!materialSet.TextRender.RenderString(labelSlot, Text))
        {
            return "Error render '" + Text + "' into the button's GUILabel";
        }

        //Create the label.
        buttonLabel = GUIElementPtr(new GUILabel(materialSet.StaticMatTextParams,
                                                 &materialSet.TextRender,
                                                 labelSlot, materialSet.StaticMatText,
                                                 materialSet.AnimateSpeed,
                                                 GUILabel::HO_CENTER, GUILabel::VO_CENTER));
        buttonLabel->SetColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
        buttonLabel->Depth = 0.01f;
        buttonLabel->ScaleBy(materialSet.TextScale);
    }
    

    //Create the button.
    MTexture2D* tex = (TexToUse == 0 ? &materialSet.ButtonTex : TexToUse);
    buttonTex = GUIElementPtr(new GUITexture(materialSet.GetAnimatedMatParams(tex), tex,
                                             materialSet.GetAnimatedMaterial(tex),
                                             true, materialSet.AnimateSpeed));
    buttonTex->SetBounds(Box2D(Vector2f(), Vector2f(ButtonSize.x, ButtonSize.y)));
    GUITexture* buttonTexPtr = (GUITexture*)buttonTex.get();
    buttonTexPtr->OnClicked = OnClick;
    buttonTexPtr->OnClicked_pData = OnClick_Data;


    //If the button has a label, create a panel to combine the label and button.
    GUIElementPtr finalButton;
    if (buttonLabel.get() == 0)
    {
        finalButton = buttonTex;
    }
    else
    {
        GUIPanel* panel = new GUIPanel();
        panel->AddElement(buttonTex);
        panel->AddElement(buttonLabel);

        finalButton = GUIElementPtr(panel);
    }

    //If the button has a description next to it, create a panel
    //    to combine the final button and description.
    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = finalButton;
    }
    else
    {
        std::string err;
        activeGUIElement = AddDescription(materialSet, finalButton, err);
        if (activeGUIElement.get() == 0)
        {
            return "Error creating description label: " + err;
        }
    }
    return "";
}

EditorButtonList::EditorButtonList(const std::vector<EditorButtonData>& buttons,
                                   DescriptionData description,
                                   float spaceBetweenButtons, Vector2f spaceAfter)
    : buttonData(buttons), buttonSpacing(spaceBetweenButtons),
      EditorObject(description, spaceAfter)
{

}
std::string EditorButtonList::InitGUIElement(EditorMaterialSet& materialSet)
{
    GUIFormattedPanel* buttonPanel = new GUIFormattedPanel();

    for (unsigned int i = 0; i < buttonData.size(); ++i)
    {
        EditorButtonData& dat = buttonData[i];
        MTexture2D* tex = (dat.Tex == 0 ? &materialSet.ButtonTex : dat.Tex);
        Vector2f buttonSize((float)tex->GetWidth() * dat.ButtonScale.x,
                            (float)tex->GetHeight() * dat.ButtonScale.y);

        GUIElementPtr buttonLabel(0),
                      buttonTex(0);

        if (!dat.Text.empty())
        {
            //First try to create the font slot to render the label.
            unsigned int textRenderWidth = (unsigned int)(buttonSize.x / materialSet.TextScale.x);
            std::string err;
            if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID, err, textRenderWidth,
                                                              materialSet.TextRenderSpaceHeight,
                                                              false,
                                                              TextureSampleSettings2D(FT_LINEAR,
                                                                                      WT_CLAMP)))
            {
                activeGUIElement = GUIElementPtr(0);
                return "Error creating text render slot for button '" + dat.Text + "'s label: " + err;
            }
            TextRenderer::FontSlot labelSlot(materialSet.FontID,
                                             materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);
            //Next try to render the text.
            if (!materialSet.TextRender.RenderString(labelSlot, dat.Text))
            {
                return "Error render '" + dat.Text + "' into the button's GUILabel";
            }

            //Create the label.
            buttonLabel = GUIElementPtr(new GUILabel(materialSet.StaticMatTextParams,
                                                     &materialSet.TextRender,
                                                     labelSlot, materialSet.StaticMatText,
                                                     materialSet.AnimateSpeed,
                                                     GUILabel::HO_CENTER, GUILabel::VO_CENTER));
            buttonLabel->SetColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
            buttonLabel->ScaleBy(materialSet.TextScale);
        }
    

        //Create the button.
        buttonTex = GUIElementPtr(new GUITexture(materialSet.GetAnimatedMatParams(tex), tex,
                                                 materialSet.GetAnimatedMaterial(tex),
                                                 true, materialSet.AnimateSpeed));
        buttonTex->SetBounds(Box2D(Vector2f(), Vector2f(-buttonSize.x, buttonSize.y)));
        GUITexture* buttonTexPtr = (GUITexture*)buttonTex.get();
        buttonTexPtr->OnClicked = dat.OnClick;
        buttonTexPtr->OnClicked_pData = dat.OnClick_pData;


        //If the button has a label, create a panel to combine the label and button.
        GUIElementPtr finalButton;
        if (buttonLabel.get() == 0)
        {
            finalButton = buttonTex;
        }
        else
        {
            GUIPanel* panel = new GUIPanel();
            panel->AddElement(buttonTex);
            panel->AddElement(buttonLabel);

            finalButton = GUIElementPtr(panel);
        }

        buttonPanel->AddObject(GUIFormatObject(finalButton, true, false, Vector2f(buttonSpacing, 0.0f)));
    }

    //If the button has a description next to it, create a panel
    //    to combine the final button and description.
    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(buttonPanel);
    }
    else
    {
        std::string err;
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(buttonPanel), err);
        if (activeGUIElement.get() == 0)
        {
            return "Error creating description label: " + err;
        }
    }

    return "";
}


EditorLabel::EditorLabel(const std::string& text, unsigned int textRenderSpaceWidth,
                         EditorObject::DescriptionData description, Vector2f offset)
    : Text(text), TextRenderSpaceWidth(textRenderSpaceWidth), EditorObject(description, offset)
{

}
std::string EditorLabel::InitGUIElement(EditorMaterialSet& materialSet)
{
    //First try to create the font slot to render the label.
    std::string err;
    if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID, err, TextRenderSpaceWidth,
                                                      materialSet.TextRenderSpaceHeight, false,
                                                      TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
    {
        activeGUIElement = GUIElementPtr(0);
        return "Error creating text render slot for label '" + Text + "': " + err;
    }
    TextRenderer::FontSlot labelSlot(materialSet.FontID,
                                     materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);

    //Next try to render the text into the slot.
    if (!materialSet.TextRender.RenderString(labelSlot, Text))
    {
        activeGUIElement = GUIElementPtr(0);
        return "Error rendering text '" + Text + "' into label slot";
    }

    activeGUIElement = GUIElementPtr(new GUILabel(materialSet.StaticMatTextParams,
                                                  &materialSet.TextRender, labelSlot,
                                                  materialSet.StaticMatText, materialSet.AnimateSpeed,
                                                  GUILabel::HO_CENTER, GUILabel::VO_CENTER));
    activeGUIElement->SetColor(materialSet.TextColor);
    activeGUIElement->ScaleBy(materialSet.TextScale);

    return "";
}

EditorImage::EditorImage(MTexture2D* tex, EditorObject::DescriptionData description,
                         Vector2f scale, Vector2f offset)
    : Tex(tex), Scale(scale), EditorObject(description, offset)
{

}
std::string EditorImage::InitGUIElement(EditorMaterialSet& set)
{
    GUITexture* texElement = new GUITexture(set.GetStaticMatParams(Tex), Tex,
                                            set.GetStaticMaterial(Tex),
                                            false, set.AnimateSpeed);
    texElement->ScaleBy(Scale);

    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(texElement);
    }
    else
    {
        std::string err;
        activeGUIElement = AddDescription(set, GUIElementPtr(texElement), err);
        if (activeGUIElement.get() == 0)
        {
            return "Error creating description label: " + err;
        }
    }

    return "";
}

EditorCollapsibleBranch::EditorCollapsibleBranch(GUIElementPtr _innerElement,
                                                 float _panelIndent, std::string titleBarName,
                                                 Vector2f spaceAfter)
    : innerElement(_innerElement), barOnly(0), fullPanel(0), panelIndent(_panelIndent),
      EditorObject(DescriptionData(titleBarName), spaceAfter)
{

}
std::string EditorCollapsibleBranch::InitGUIElement(EditorMaterialSet& set)
{
    MTexture2D* titleTex = &set.CollapsibleEditorTitleBarTex;
    GUITexture* titleBar = new GUITexture(set.GetAnimatedMatParams(titleTex), titleTex,
                                          set.GetAnimatedMaterial(titleTex),
                                          true, set.AnimateSpeed);
    titleBar->OnClicked_pData = this;
#pragma warning(disable: 4100)
    titleBar->OnClicked = [](GUITexture* titleButton, Vector2f mousePos, void* pData)
    {
        EditorCollapsibleBranch* thisB = (EditorCollapsibleBranch*)pData;
        thisB->Toggle();
    };
#pragma warning(default: 4100)


    if (DescriptionLabel.Text.empty())
    {
        barOnly = GUIElementPtr(titleBar);
    }
    else
    {
        //First try to create the font slot to render the label.
        std::string err;
        unsigned int renderSpaceWidth = (unsigned int)((float)titleTex->GetWidth() / set.TextScale.x);
        if (!set.TextRender.CreateTextRenderSlots(set.FontID, err, renderSpaceWidth,
                                                  set.TextRenderSpaceHeight, false,
                                                  TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
        {
            return "Error creating text render slot for description '" +
                        DescriptionLabel.Text + "': " + err;
        }
        TextRenderer::FontSlot labelSlot(set.FontID, set.TextRender.GetNumbSlots(set.FontID) - 1);
        //Next try to render the text.
        if (!set.TextRender.RenderString(labelSlot, DescriptionLabel.Text))
        {
            return "Error rendering '" + DescriptionLabel.Text + "' into the description label: " + err;
        }
        //Make the label.
        GUIElementPtr label(new GUILabel(set.StaticMatTextParams, &set.TextRender,
                                         labelSlot, set.StaticMatText, set.AnimateSpeed,
                                         GUILabel::HO_CENTER, GUILabel::VO_CENTER));
        label->SetColor(set.CollapsibleEditorTitleTextCol);
        label->ScaleBy(set.TextScale);
        label->Depth += 0.001f;


        //Combine the title bar and label together into a panel.
        GUIPanel* titleBarPanel = new GUIPanel();
        titleBarPanel->AddElement(GUIElementPtr(titleBar));
        titleBarPanel->AddElement(label);
        label->SetPosition(Vector2f(0.0f, 0.0f));
        barOnly = GUIElementPtr(titleBarPanel);
    }
    

    //Now make an outer panel that includes the inner element.
    GUIFormattedPanel* fullPanelPtr = new GUIFormattedPanel();
    fullPanelPtr->AddObject(GUIFormatObject(barOnly, false, true, Vector2f(10.0f, 0.0f)));
    fullPanelPtr->AddObject(GUIFormatObject(innerElement));
    fullPanel = GUIElementPtr(fullPanelPtr);


    //Start with just the title bar.
    activeGUIElement = barOnly;
    return "";
}

#pragma warning (disable:4100)
bool EditorCollapsibleBranch::Update(float elapsed, Vector2f panelRelMouse)
{
    bool b = didActiveElementChange;
    didActiveElementChange = false;
    return b;
}
#pragma warning (default:4100)

void EditorCollapsibleBranch::Toggle(void)
{
    didActiveElementChange = true;

    if (activeGUIElement.get() == barOnly.get())
    {
        activeGUIElement = fullPanel;
        //TODO: See whether the following line is actually necessary.
        ((GUIFormattedPanel*)fullPanel.get())->DidBoundsChange = true;
    }
    else
    {
        assert(activeGUIElement.get() == fullPanel.get());
        activeGUIElement = barOnly;
    }
}