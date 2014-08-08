#include "GUILabel.h"

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

    float invWidth = 1.0f / info.Cam->Info.Width,
          invHeight = 1.0f / info.Cam->Info.Height;

    Vector2f pos(invWidth, invHeight);
    pos.MultiplyComponents(Vector2f(center.x, center.y));
    Vector2f scale(invWidth, invHeight);
    scale.MultiplyComponents(Scale.ComponentProduct(Vector2f(dimensions.x, dimensions.y)));

    GetQuad()->SetPos(pos);
    GetQuad()->SetSize(scale * 0.5f);
    GetQuad()->SetRotation(0.0f);

    Params.Texture2DUniforms[textSampler].Texture = TextRender->GetRenderedString(TextRenderSlot)->GetTextureHandle();

    if (GetQuad()->Render(info, Params, *RenderMat)) return "";
    return "Error rendering label with text '" + text + "': " + RenderMat->GetErrorMsg();
}