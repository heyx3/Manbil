#include "GUIElement.h"

#include "GUIMaterials.h"
#include "../Materials/Data Nodes/DataNodeIncludes.h"


DrawingQuad * GUIElement::quad = 0;


bool GUIElement::IsLocalInsideBounds(Vector2i pos) const
{
    Vector2i halfSize = GetCollisionDimensions() / 2;
    return (pos.x >= halfSize.x) || (pos.y >= halfSize.y) ||
           (pos.x <= halfSize.x) || (pos.y <= halfSize.y);
}

void GUIElement::SetUpQuad(const RenderInfo & info, Vector2f pos, Vector2f scale, float rot)
{
    float invWidth = 1.0f / info.Cam->Info.Width,
          invHeight = 1.0f / info.Cam->Info.Height;

    pos.MultiplyComponents(Vector2f(invWidth, invHeight));
    scale.MultiplyComponents(Vector2f(invWidth, invHeight));

    quad->SetPos(pos);
    quad->SetSize(scale * 0.5f);
    quad->SetRotation(rot);
}

void GUIElement::Update(float elapsed)
{
    this->CustomUpdate(elapsed);

    auto found = Params.FloatUniforms.find(GUIMaterials::DynamicQuadDraw_TimeLerp);
    if (found == Params.FloatUniforms.end()) return;

    float val = Params.FloatUniforms[GUIMaterials::DynamicQuadDraw_TimeLerp].Value[0];

    //Update the time lerp value and stop its movement if it hits 0 or 1.
    if (CurrentTimeLerpSpeed > 0.0f)
    {
        val += elapsed * CurrentTimeLerpSpeed;
        if (val >= 1.0f)
        {
            CurrentTimeLerpSpeed = 0.0f;
            val = 1.0f;
        }
    }
    else if (CurrentTimeLerpSpeed < 0.0f)
    {
        val += elapsed * CurrentTimeLerpSpeed;
        if (val <= 0.0f)
        {
            CurrentTimeLerpSpeed = 0.0f;
            val = 0.0f;
        }
    }

    Params.FloatUniforms[GUIMaterials::DynamicQuadDraw_TimeLerp].Value[0] = val;
}