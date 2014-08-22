#include "GUILabel.h"

#include "../GUIMaterials.h"
#include "../../Materials/Data Nodes/DataNodeIncludes.h"



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

Vector2f GUILabel::GetCollisionDimensions(void) const
{
    return Vector2f((float)dimensions.x, (float)dimensions.y).ComponentProduct(Scale) + Vector2f(1, 1);
}
std::string GUILabel::Render(float elapsedTime, const RenderInfo & info)
{
    //Don't bother doing any rendering if there's no text to display.
    if (text.empty()) return "";


    Vector2i rendSize = TextRender->GetSlotRenderSize(TextRenderSlot);

    Vector2f textOffset;
    switch (OffsetHorz)
    {
        case HO_LEFT:
            textOffset.x = rendSize.x * 0.5f;
            break;
        case HO_CENTER:
            textOffset.x = (rendSize.x * 0.5f) - (dimensions.x * 0.25f);
            break;
        case HO_RIGHT:
            textOffset.x = (rendSize.x * 0.5f) - (dimensions.x * 0.5f);
            break;
        default: assert(false);
    }
    switch (OffsetVert)
    {
        case VO_TOP:
            textOffset.y = rendSize.y * 0.5f;
            break;
        case VO_CENTER:
            textOffset.y = (rendSize.y * 0.5f) - (dimensions.y * 0.25f);
            break;
        case VO_BOTTOM:
            textOffset.y = (rendSize.y * 0.5f) - (dimensions.y * 0.5f);
            break;
        default: assert(false);
    }
    
    //Flip the Y.
    textOffset.y = -textOffset.y;

    Vector2f textOffsetF = Vector2f((float)textOffset.x * Scale.x, (float)textOffset.y * Scale.y);

    SetUpQuad(info, center + textOffsetF, Scale.ComponentProduct(ToV2f(rendSize)));
    
    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture =
        TextRender->GetRenderedString(TextRenderSlot)->GetTextureHandle();

    if (GetQuad()->Render(info, Params, *RenderMat)) return "";
    return "Error rendering label with text '" + text + "': " + RenderMat->GetErrorMsg();
}