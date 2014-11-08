#include "EditorObject.h"

#include "../Rendering/GUI/GUI Elements/GUILabel.h"
#include "../Rendering/GUI/GUI Elements/GUIFormattedPanel.h"


GUIElementPtr EditorObject::AddDescription(EditorMaterialSet & set, GUIElementPtr element) const
{
    //First try to create the font slot to render the label.
    if (!set.TextRender.CreateTextRenderSlots(set.FontID, DescriptionLabel.TextRenderWidth,
                                              set.TextRenderSpaceHeight, false,
                                              TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
    {
        ErrorMsg = "Error creating text render slot for description '" +
                       DescriptionLabel.Text + "': " + set.TextRender.GetError();
        return GUIElementPtr(0);
    }
    TextRenderer::FontSlot labelSlot(set.FontID, set.TextRender.GetNumbSlots(set.FontID) - 1);

    //Next try to render the text.
    if (!set.TextRender.RenderString(labelSlot, DescriptionLabel.Text))
    {
        ErrorMsg = "Error rendering '" + DescriptionLabel.Text + "' into the description label: " +
                       set.TextRender.GetError();
        return GUIElementPtr(0);
    }

    //Make the label.
    GUIElementPtr label(new GUILabel(set.StaticMatTextParams, &set.TextRender,
                                     labelSlot, set.StaticMatText,
                                     set.AnimateSpeed, GUILabel::HO_CENTER, GUILabel::VO_CENTER));
    label->SetColor(set.TextColor);
    label->ScaleBy(set.TextScale);


    //Make the panel.
    GUIFormattedPanel* panel = new GUIFormattedPanel();
    panel->AddObject(GUIFormatObject(DescriptionLabel.IsOnLeft ? label : element,
                                     true, false, Vector2f(DescriptionLabel.Spacing, 0.0f)));
    panel->AddObject(GUIFormatObject(DescriptionLabel.IsOnLeft ? element : label));
    return GUIElementPtr(panel);
}