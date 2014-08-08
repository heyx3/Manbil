#pragma once

#include "../Helper Classes/DrawingQuad.h"


//Represents a single element in a UI.
//It has a background texture and a collision box relative to the background image.
class GUIElement
{
public:

    typedef std::shared_ptr<GUIElement> Ptr;

    UniformDictionary Params;


    virtual ~GUIElement(void) { }


    //The center of this element's collision box.
    virtual Vector2i GetCollisionCenter(void) const = 0;
    //The size of this element's collision box.
    virtual Vector2i GetCollisionDimensions(void) const = 0;

    //Moves this element by the given amount.
    virtual void MoveElement(Vector2i moveAmount) = 0;
    //Sets this element's position.
    virtual void SetPosition(Vector2i newPos) = 0;

    //Scales this element by the given amount (may be approximate).
    virtual void ScaleBy(Vector2f scaleAmount) = 0;


    virtual void Update(float elapsedTime) { }

    //Returns an error message, or the empty string if everything went fine.
    virtual std::string Render(float elapsedTime, const RenderInfo & info) = 0;

    //Raised when the mouse clicks on something.
    //The given Vector2i is the mouse position relative to this element's center.
    virtual void OnMouseClick(Vector2i mouse_centerOffset) { }
    //Raised when the mouse drags this element.
    //The given Vector2i instances are relative to this element's center.
    virtual void OnMouseDrag(Vector2i originalPos_centerOffset,
                             Vector2i currentPos_centerOffset) { }
    //Raised when the mouse releases over this element.
    //Ths given Vector2i is the current mouse position relative to this element's center.
    virtual void OnMouseRelease(Vector2i mouse_centerOffset) { }

    //Gets whether the given position is inside this element's bounds.
    bool IsInsideBounds(Vector2i pos) const;


protected:

    static DrawingQuad * GetQuad(void) { if (quad == 0) quad = new DrawingQuad(); return quad; }

private:

    static DrawingQuad * quad;
};