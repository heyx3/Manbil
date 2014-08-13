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

void GUIElement::Update(float elapsed, Vector2i relativeMouse)
{
    this->CustomUpdate(elapsed, relativeMouse);

    if (!UsesTimeLerp()) return;


    //Update the time lerp value.

    float timeLerp = GetTimeLerp();
    isMousedOver = IsLocalInsideBounds(relativeMouse);

    //If this element isn't animating, check whether it's being moused over.
    if (CurrentTimeLerpSpeed == 0.0f)
    {
        timeLerp = (isMousedOver ? 0.5f : 0.0f);
    }
    //Otherwise, update its animation.
    else
    {
        timeLerp += elapsed * CurrentTimeLerpSpeed;

        if (timeLerp <= 0.0f)
        {
            CurrentTimeLerpSpeed = 0.0f;
            timeLerp = 0.00001f;
        }
        else if (timeLerp >= 1.0f)
        {
            CurrentTimeLerpSpeed = -CurrentTimeLerpSpeed;
            timeLerp = 0.999999f;
        }
    }

    //Update the time lerp parameter with the new value.
    SetTimeLerp(timeLerp);
}