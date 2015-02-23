#include "GUILabel.h"

#include "../GUIMaterials.h"
#include "../../Materials/Data Nodes/DataNodes.hpp"


GUILabel::GUILabel(const UniformDictionary& params, TextRenderer* _textRenderer,
                   TextRenderer::FontSlot textSlot,
                   Material* material, float timeSpeed,
                   HorizontalOffsets _offsetH, VerticalOffsets _offsetV)
    : offsetH(_offsetH), offsetV(_offsetV), textRenderer(_textRenderer),
      RenderMat(material), textRenderSlot(textSlot), text(_textRenderer->GetString(textSlot)),
      GUIElement(params, timeSpeed)
{
    if (!text.empty() && textRenderer != 0)
    {
        dimensions = ToV2f(textRenderer->GetSlotBoundingSize(textSlot));
    }
}

Box2D GUILabel::GetBounds(void) const
{
    //If this label doesn't currently contain text, use the smallest-possible bounding box.
    if (text.empty())
    {
        float maxY = (float)textRenderer->GetMaxCharacterSize(textRenderSlot.FontID).y;
        return Box2D(GetAnchorToTextCenter(), Vector2f(0.0001f, GetScale().y * maxY));
    }
    else
    {
        return Box2D(GetAnchorToTextCenter(), dimensions.ComponentProduct(GetScale()));
    }
}

Vector2f GUILabel::GetAnchorToTextCenter(void) const
{
    Vector2f rendSize = ToV2f(textRenderer->GetSlotRenderSize(textRenderSlot));
    Vector2f textOffset;

    switch (offsetH)
    {
        case HO_LEFT:
            textOffset.x = dimensions.x * 0.5f;
            break;
        case HO_CENTER:
            textOffset.x = 0.0f;
            break;
        case HO_RIGHT:
            textOffset.x = dimensions.x * -0.5f;
            break;
        default: assert(false);
    }
    switch (offsetV)
    {
        case VO_TOP:
            textOffset.y = dimensions.y * -0.5f;
            break;
        case VO_CENTER:
            textOffset.y = 0.0f;
            break;
        case VO_BOTTOM:
            textOffset.y = dimensions.y * 0.5f;
            break;
        default: assert(false);
    }

    return textOffset.ComponentProduct(GetScale());
}
Vector2f GUILabel::GetAnchorToRenderCenter(void) const
{
    Vector2f rendSize = ToV2f(textRenderer->GetSlotRenderSize(textRenderSlot));
    Vector2f textOffset((0.5f * dimensions.x) + ((0.5f * rendSize.x) - dimensions.x),
                        0.5f * dimensions.y);

    switch (offsetH)
    {
        case HO_LEFT:
            textOffset.x += dimensions.x * 0.5f;
            break;
        case HO_CENTER:
            textOffset.x += 0.0f;
            break;
        case HO_RIGHT:
            textOffset.x -= dimensions.x * 0.5f;
            break;
        default: assert(false);
    }
    switch (offsetV)
    {
        case VO_TOP:
            textOffset.y -= dimensions.y * 0.5f;
            break;
        case VO_CENTER:
            textOffset.y += 0.0f;
            break;
        case VO_BOTTOM:
            textOffset.y += dimensions.y * 0.5f;
            break;
        default: assert(false);
    }

    return textOffset.ComponentProduct(GetScale());
}

bool GUILabel::SetText(std::string newText)
{
    if (!textRenderer->RenderString(textRenderSlot, newText))
    {
        return false;
    }

    text = newText;
    dimensions = ToV2f(textRenderer->GetSlotBoundingSize(textRenderSlot));
    DidBoundsChange = true;
    return true;
}
void GUILabel::SetTextRenderSlot(TextRenderer::FontSlot newSlot)
{
    DidBoundsChange = true;
    
    textRenderSlot = newSlot;
    text = textRenderer->GetString(textRenderSlot);
    dimensions = ToV2f(textRenderer->GetSlotBoundingSize(textRenderSlot));
}

#pragma warning(disable: 4100)
void GUILabel::Render(float elapsedTime, const RenderInfo& info)
{
    //Don't bother doing any rendering if there's no text to display.
    if (text.empty())
    {
        return;
    }

    Vector2f labelDrawSize = GetScale();
    labelDrawSize.MultiplyComponents(ToV2f(textRenderer->GetSlotRenderSize(textRenderSlot)));
    SetUpQuad(Box2D(GetPos() + GetAnchorToRenderCenter(), labelDrawSize), Depth);
    
    RenderObjHandle texHandle = textRenderer->GetRenderedString(textRenderSlot)->GetTextureHandle();
    Params.Texture2Ds[GUIMaterials::QuadDraw_Texture2D].Texture = texHandle;

    RenderMat->GetBlendMode().EnableMode();
    GetQuad()->Render(info, Params, *RenderMat);
}
#pragma warning(default: 4100)