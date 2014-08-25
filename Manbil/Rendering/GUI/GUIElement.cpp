#include "GUIElement.h"

#include "GUIMaterials.h"
#include "../Materials/Data Nodes/DataNodeIncludes.h"


DrawingQuad * GUIElement::quad = 0;


void GUIElement::SetBounds(Vector2f min, Vector2f max)
{
    SetPosition((min + max) * 0.5f);

    Vector2f dims = GetCollisionDimensions();
    Vector2f newSize = max - min;
    Vector2f delta(newSize.x / dims.x, newSize.y / dims.y);
    ScaleBy(delta);
}

bool GUIElement::IsLocalInsideBounds(Vector2f pos) const
{
    Vector2f halfSize = GetCollisionDimensions() * 0.5f;
    return (pos.x >= -halfSize.x && pos.x <= halfSize.x &&
            pos.y >= -halfSize.y && pos.y <= halfSize.y);
}

void GUIElement::SetUpQuad(const RenderInfo & info, Vector2f pos, float depth, Vector2f scale, float rot)
{
    GetQuad()->SetPos(pos);
    GetQuad()->SetDepth(depth);
    GetQuad()->SetSize(scale * 0.5f);
    GetQuad()->SetRotation(rot);
}

void GUIElement::Update(float elapsed, Vector2f relativeMouse)
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

        if (timeLerp <= 0.0f ||
            (CurrentTimeLerpSpeed < 0.0f && isMousedOver && timeLerp <= 0.5f))
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