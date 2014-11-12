#include "GUILabel.h"

#include "../GUIMaterials.h"
#include "../../Materials/Data Nodes/DataNodeIncludes.h"



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
    if (textRenderer->RenderString(textRenderSlot, newText))
    {
        text = newText;
        dimensions = ToV2f(textRenderer->GetSlotBoundingSize(textRenderSlot));
        DidBoundsChange = true;
        return true;
    }
    else return false;
}
void GUILabel::SetTextRenderSlot(TextRenderer::FontSlot newSlot)
{
    DidBoundsChange = true;
    
    textRenderSlot = newSlot;
    text = textRenderer->GetString(textRenderSlot);
    dimensions = ToV2f(textRenderer->GetSlotBoundingSize(textRenderSlot));
}
std::string GUILabel::Render(float elapsedTime, const RenderInfo & info)
{
    //Don't bother doing any rendering if there's no text to display.
    if (text.empty()) return "";


    SetUpQuad(Box2D(GetPos() + GetAnchorToRenderCenter(),
                    GetScale().ComponentProduct(ToV2f(textRenderer->GetSlotRenderSize(textRenderSlot)))),
              Depth);
    
    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture =
        textRenderer->GetRenderedString(textRenderSlot)->GetTextureHandle();

    if (GetQuad()->Render(info, Params, *RenderMat)) return "";
    else return "Error rendering label with text '" + text + "': " + RenderMat->GetErrorMsg();
}