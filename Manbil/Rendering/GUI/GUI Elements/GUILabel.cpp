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

Vector2i GUILabel::GetCollisionDimensions(void) const
{
    return Vector2f((float)dimensions.x, (float)dimensions.y).ComponentProduct(Scale).CastToInt() + Vector2i(1, 1);
}
std::string GUILabel::Render(float elapsedTime, const RenderInfo & info)
{
    Vector2i textOffset;
    switch (OffsetHorz)
    {
        case HO_LEFT:
            textOffset.x = 0;
            break;
        case HO_CENTER:
            textOffset.x = dimensions.x / 2;
            break;
        case HO_RIGHT:
            textOffset.x = dimensions.x;
            break;
        default: assert(false);
    }
    switch (OffsetVert)
    {
        case VO_TOP:
            textOffset.y = 0;
            break;
        case VO_CENTER:
            textOffset.y = dimensions.y / 2;
            break;
        case VO_BOTTOM:
            textOffset.y = dimensions.y;
            break;
        default: assert(false);
    }

    Vector2f textOffsetF = Vector2f((float)textOffset.x * Scale.x,
                                    (float)textOffset.y * Scale.y);

    SetUpQuad(info, ToV2f(center) + textOffsetF, Scale.ComponentProduct(ToV2f(dimensions)));
    
    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture =
        TextRender->GetRenderedString(TextRenderSlot)->GetTextureHandle();

    if (GetQuad()->Render(info, Params, *RenderMat)) return "";
    return "Error rendering label with text '" + text + "': " + RenderMat->GetErrorMsg();
}