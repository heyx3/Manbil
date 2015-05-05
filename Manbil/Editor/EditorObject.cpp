#include "EditorObject.h"

#include "../Rendering/GUI/GUI Elements/GUILabel.h"
#include "../Rendering/GUI/GUI Elements/GUIFormattedPanel.h"



GUIElementPtr EditorObject::AddDescription(EditorMaterialSet& set, GUIElementPtr element,
                                           std::string& outError) const
{
    //First try to create the font slot to render the label.
    std::string outErr;
    TextRenderer::FontSlot labelSlot = set.CreateSlot(DescriptionLabel.TextRenderWidth, outErr,
                                                      FT_LINEAR, false);
    if (!outErr.empty())
    {
        outError = "Error creating text render slot for description '" +
                        DescriptionLabel.Text + "': " + outErr;
        return GUIElementPtr(0);
    }

    //Next try to render the text.
    if (!set.TextRender.RenderString(labelSlot, DescriptionLabel.Text))
    {
        outError = "Error rendering '" + DescriptionLabel.Text + "' into the description label.";
        return GUIElementPtr(0);
    }

    //Make the label.
    GUIElementPtr label(new GUILabel(set.StaticMatTextParams, &set.TextRender,
                                     labelSlot, set.StaticMatText,
                                     set.AnimateSpeed, GUILabel::HO_CENTER, GUILabel::VO_CENTER));
    ((GUILabel*)label.get())->DeleteSlotWhenDeleted = true;
    label->SetColor(set.TextColor);
    label->ScaleBy(set.TextScale);


    //Make the panel.
    GUIFormattedPanel* panel = new GUIFormattedPanel();
    panel->AddObject(GUIFormatObject(DescriptionLabel.IsOnLeft ? label : element,
                                     true, false, Vector2f(DescriptionLabel.Spacing, 0.0f)));
    panel->AddObject(GUIFormatObject(DescriptionLabel.IsOnLeft ? element : label));
    return GUIElementPtr(panel);
}