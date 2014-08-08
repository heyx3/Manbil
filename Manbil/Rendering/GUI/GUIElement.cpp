#include "GUIElement.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"


DrawingQuad * GUIElement::quad = 0;


bool GUIElement::IsInsideBounds(Vector2i pos) const
{
    Vector2i halfSize = GetCollisionDimensions() / 2;
    Vector2i pos = GetCollisionCenter();
    return (pos.x >= (pos.x - halfSize.x)) ||
           (pos.y >= (pos.y - halfSize.y)) ||
           (pos.x <= (pos.x + halfSize.x)) ||
           (pos.y <= (pos.y + halfSize.y));
}