#include "GUILabel.h"

#include "../GUIMaterials.h"
#include "../../Materials/Data Nodes/DataNodeIncludes.h"



Vector2f GUILabel::GetPos(void) const
{
    Vector2f anchor = GUIElement::GetPos();

    Vector2f outCenter;
    Vector2f halfDims = dimensions.ComponentProduct(GetScale()) * 0.5f;

    switch (offsetH)
    {
        case HO_LEFT:
            outCenter.x = anchor.x + halfDims.x;
            break;
        case HO_CENTER:
            outCenter.x = anchor.x;
            break;
        case HO_RIGHT:
            outCenter.x = anchor.x - halfDims.x;
            break;
        default: assert(false);
    }
    switch (offsetV)
    {
        case VO_TOP:
            outCenter.y = anchor.y - halfDims.y;
            break;
        case VO_CENTER:
            outCenter.y = anchor.y;
            break;
        case VO_BOTTOM:
            outCenter.y = anchor.y + halfDims.y;
            break;
        default: assert(false);
    }

    return outCenter;
}
Box2D GUILabel::GetBounds(void) const
{
    Vector2f pos = GUIElement::GetPos() + GetTextOffset();

    //If this label doesn't currently contain text, use the smallest-possible bounding box.
    if (text.empty())
    {
        float maxY = (float)textRenderer->GetMaxCharacterSize(textRenderSlot.FontID).y;
        return Box2D(pos, Vector2f(0.0001f, GetScale().y * maxY));
    }
    else
    {
        return Box2D(pos, dimensions.ComponentProduct(GetScale()));
    }
}

Vector2f GUILabel::GetTextOffset(void) const
{
    Vector2i rendSize = textRenderer->GetSlotRenderSize(textRenderSlot);
    Vector2f textOffset;

    switch (offsetH)
    {
        case HO_LEFT:
            textOffset.x = rendSize.x * 0.5f;
            break;
        case HO_CENTER:
            textOffset.x = (rendSize.x * 0.5f) - (dimensions.x * 0.5f);
            break;
        case HO_RIGHT:
            textOffset.x = (rendSize.x * 0.5f) - dimensions.x;
            break;
        default: assert(false);
    }
    switch (offsetV)
    {
        case VO_TOP:
            textOffset.y = rendSize.y * 0.5f;
            break;
        case VO_CENTER:
            textOffset.y = (rendSize.y * 0.5f) - (dimensions.y * 0.5f);
            break;
        case VO_BOTTOM:
            textOffset.y = (rendSize.y * 0.5f) - dimensions.y;
            break;
        default: assert(false);
    }

    //Flip the Y.
    textOffset.y = -textOffset.y;

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
    SetBoundsChanged();
    
    textRenderSlot = newSlot;
    text = textRenderer->GetString(textRenderSlot);
    dimensions = ToV2f(textRenderer->GetSlotBoundingSize(textRenderSlot));
}
std::string GUILabel::Render(float elapsedTime, const RenderInfo & info)
{
    //Don't bother doing any rendering if there's no text to display.
    if (text.empty()) return "";


    SetUpQuad(Box2D(GUIElement::GetPos() + GetTextOffset(),
                    GetScale().ComponentProduct(ToV2f(textRenderer->GetSlotRenderSize(textRenderSlot)))),
              Depth);
    
    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture =
        textRenderer->GetRenderedString(textRenderSlot)->GetTextureHandle();

    if (GetQuad()->Render(info, Params, *RenderMat)) return "";
    else return "Error rendering label with text '" + text + "': " + RenderMat->GetErrorMsg();
}