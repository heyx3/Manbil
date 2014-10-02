#include "EditorObjects.h"

#include "../Rendering/GUI/GUI Elements/GUIPanel.h"


std::string EditorObject::ErrorMsg = "";


bool CheckboxValue::InitGUIElement(EditorMaterialSet & materialSet)
{
   MTexture2D *boxTex = &materialSet.CheckBoxBackgroundTex,
              *checkTex = &materialSet.CheckBoxCheckTex;
   GUITexture boxGUITex(materialSet.GetAnimatedMatParams(boxTex), boxTex,
                        materialSet.GetAnimatedMaterial(boxTex), true, materialSet.AnimateSpeed),
              checkGUITex(materialSet.GetAnimatedMatParams(checkTex), checkTex,
                          materialSet.GetAnimatedMaterial(checkTex), false, materialSet.AnimateSpeed);
    GUICheckbox * box = new GUICheckbox(materialSet.StaticMatGreyParams, boxGUITex, checkGUITex);

    box->SetChecked(DefaultValue, false);
    box->OnClicked = OnBoxClicked;
    box->OnClicked_Data = OnBoxClicked_Data;

    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(box);
    }
    else
    {
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(box));
        if (activeGUIElement.get() == 0)
            return false;
    }
    return true;
}

bool DropdownValues::InitGUIElement(EditorMaterialSet & materialSet)
{
    GUITexture itemListBackground(materialSet.GetStaticMatParams(&materialSet.SelectionBoxBackgroundTex),
                                  &materialSet.SelectionBoxBackgroundTex,
                                  materialSet.GetStaticMaterial(&materialSet.SelectionBoxBackgroundTex),
                                  false, materialSet.AnimateSpeed);
    GUITexture selectedItemBox(materialSet.GetAnimatedMatParams(&materialSet.SelectionBoxBoxTex),
                               &materialSet.SelectionBoxBoxTex,
                               materialSet.GetAnimatedMaterial(&materialSet.SelectionBoxBoxTex),
                               false, materialSet.AnimateSpeed);
    GUISelectionBox* box = new GUISelectionBox(materialSet.StaticMatGreyParams,
                                               &materialSet.TextRender,
                                               selectedItemBox, materialSet.FontID,
                                               Vector2u(selectedItemBox.Tex->GetWidth(),
                                               selectedItemBox.Tex->GetHeight()),
                                               TextureSampleSettings2D(FT_LINEAR, WT_CLAMP),
                                               materialSet.StaticMatColor, GUILabel::HO_LEFT,
                                               itemListBackground, Items, 0, true, materialSet.AnimateSpeed);
    if (box->BoxElement.Mat == 0)
    {
        ErrorMsg = "Error generating text slots for dropdown box: " + box->TextRender->GetError();
        delete box;
        activeGUIElement = GUIElementPtr(0);
        return false;
    }

    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(box);
    }
    else
    {
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(box));
        if (activeGUIElement.get() == 0)
            return false;
    }
    return true;
}

bool EditorButton::InitGUIElement(EditorMaterialSet & materialSet)
{
    //First try to create the font slot to render the label.
    if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID,
                                                      (unsigned int)(ButtonSize.x / materialSet.TextScale.x),
                                                      materialSet.TextRenderSpaceHeight,
                                                      false, TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
    {
        ErrorMsg = "Error creating text render slot for button '" + Text +
                       "'s label: " + materialSet.TextRender.GetError();
        activeGUIElement = GUIElementPtr(0);
        return false;
    }
    TextRenderer::FontSlot labelSlot(materialSet.FontID,
                                     materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);
    //Next try to render the text.
    if (!materialSet.TextRender.RenderString(labelSlot, Text))
    {
        ErrorMsg = "Error render '" + Text + "' into the button's GUILabel: " + materialSet.TextRender.GetError();
        return false;
    }

    //Create the button.
    buttonTex = GUIElementPtr(new GUITexture(materialSet.GetAnimatedMatParams(&materialSet.ButtonTex),
                                             &materialSet.ButtonTex,
                                             materialSet.GetAnimatedMaterial(&materialSet.ButtonTex),
                                             true, materialSet.AnimateSpeed));
    buttonTex->SetBounds(ButtonSize * -0.5f, ButtonSize * 0.5f);
    GUITexture* buttonTexPtr = (GUITexture*)buttonTex.get();
    buttonTexPtr->OnClicked = OnClick;
    buttonTexPtr->OnClicked_pData = OnClick_Data;

    //Create the label.
    buttonLabel = GUIElementPtr(new GUILabel(materialSet.StaticMatGreyParams, &materialSet.TextRender,
                                             labelSlot, materialSet.StaticMatGrey,
                                             materialSet.AnimateSpeed, GUILabel::HO_CENTER, GUILabel::VO_CENTER));
    buttonLabel->SetColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
    buttonLabel->ScaleBy(materialSet.TextScale);

    GUIPanel* panel = new GUIPanel(materialSet.StaticMatGreyParams, ButtonSize, materialSet.AnimateSpeed);
    panel->AddElement(buttonTex);
    panel->AddElement(buttonLabel);


    if (DescriptionLabel.Text.empty())
    {
        activeGUIElement = GUIElementPtr(panel);
    }
    else
    {
        activeGUIElement = AddDescription(materialSet, GUIElementPtr(panel));
        if (activeGUIElement.get() == 0)
            return false;
    }
    return true;
}

bool EditorLabel::InitGUIElement(EditorMaterialSet & materialSet)
{
    //First try to create the font slot to render the label.
    if (!materialSet.TextRender.CreateTextRenderSlots(materialSet.FontID,
                                                      TextRenderSpaceWidth,
                                                      materialSet.TextRenderSpaceHeight,
                                                      false, TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
    {
        ErrorMsg = "Error creating text render slot for label '" + Text +
                       "': " + materialSet.TextRender.GetError();
        activeGUIElement = GUIElementPtr(0);
        return false;
    }
    TextRenderer::FontSlot labelSlot(materialSet.FontID,
                                     materialSet.TextRender.GetNumbSlots(materialSet.FontID) - 1);

    activeGUIElement = GUIElementPtr(new GUILabel(materialSet.StaticMatGreyParams, &materialSet.TextRender,
                                                  labelSlot, materialSet.StaticMatGrey,
                                                  materialSet.AnimateSpeed, GUILabel::HO_CENTER, GUILabel::VO_CENTER));
    activeGUIElement->SetColor(materialSet.TextColor);
    activeGUIElement->ScaleBy(materialSet.TextScale);

    return true;
}