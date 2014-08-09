#include "GUIElement.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"


DrawingQuad * GUIElement::quad = 0;


bool GUIElement::IsInsideBounds(Vector2i pos) const
{
    Vector2i halfSize = GetCollisionDimensions() / 2;
    Vector2i cent = GetCollisionCenter();
    return (pos.x >= (cent.x - halfSize.x)) ||
           (pos.y >= (cent.y - halfSize.y)) ||
           (pos.x <= (cent.x + halfSize.x)) ||
           (pos.y <= (cent.y + halfSize.y));
}