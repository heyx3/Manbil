#include "GUILabel.h"

#include "../GUIMaterials.h"
#include "../../Materials/Data Nodes/DataNodeIncludes.h"



Vector2f GUILabel::GetCollisionCenter(void) const
{
    Vector2f outCenter;
    Vector2f halfDims = dimensions.ComponentProduct(Scale) * 0.5f;

    switch (OffsetHorz)
    {
        case HO_LEFT:
            outCenter.x = center.x + halfDims.x;
            break;
        case HO_CENTER:
            outCenter.x = center.x;
            break;
        case HO_RIGHT:
            outCenter.x = center.x - halfDims.x;
            break;
        default: assert(false);
    }
    switch (OffsetVert)
    {
        case VO_TOP:
            outCenter.y = center.y - halfDims.y;
            break;
        case VO_CENTER:
            outCenter.y = center.y;
            break;
        case VO_BOTTOM:
            outCenter.y = center.y + halfDims.y;
            break;
        default: assert(false);
    }

    return outCenter;
}
Vector2f GUILabel::GetCollisionDimensions(void) const
{
    Vector2f dims = dimensions.ComponentProduct(Scale);
    if (dims.x == 0.0f) dims.x = 1.0f;
    if (dims.y == 0.0f) dims.y = Scale.y * (float)TextRender->GetSlotRenderSize(TextRenderSlot).y;
    return dims;
}

Vector2f GUILabel::GetTextOffset(void) const
{
    Vector2i rendSize = TextRender->GetSlotRenderSize(TextRenderSlot);
    Vector2f textOffset;

    switch (OffsetHorz)
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
    switch (OffsetVert)
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

    return textOffset.ComponentProduct(Scale);
}

bool GUILabel::SetText(std::string newText)
{
    if (TextRender->RenderString(TextRenderSlot, newText))
    {
        text = newText;
        dimensions = TextRender->GetSlotBoundingSize(TextRenderSlot);
        return true;
    }
    else return false;
}

std::string GUILabel::Render(float elapsedTime, const RenderInfo & info)
{
    //Don't bother doing any rendering if there's no text to display.
    if (text.empty()) return "";


    Vector2f textOffset = GetTextOffset();
    Vector2i rendSize = TextRender->GetSlotRenderSize(TextRenderSlot);

    SetUpQuad(center + textOffset, Depth, Scale.ComponentProduct(ToV2f(rendSize)));
    
    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture =
        TextRender->GetRenderedString(TextRenderSlot)->GetTextureHandle();

    if (GetQuad()->Render(info, Params, *RenderMat)) return "";
    return "Error rendering label with text '" + text + "': " + RenderMat->GetErrorMsg();
}