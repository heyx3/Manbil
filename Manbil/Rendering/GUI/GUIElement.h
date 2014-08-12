#pragma once

#include "../Helper Classes/DrawingQuad.h"


//Represents a single element in a UI.
//Note that mouse events should be raised for all GUI elements, not just ones that are touching the mouse.
class GUIElement
{
public:

    typedef std::shared_ptr<GUIElement> Ptr;

    UniformDictionary Params;
    float TimeLerpSpeed;


    GUIElement(float timeLerpSpeed = 1.0f) : TimeLerpSpeed(timeLerpSpeed) { }
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


    virtual void Update(float elapsedTime);


    //Returns an error message, or the empty string if everything went fine.
    virtual std::string Render(float elapsedTime, const RenderInfo & info) = 0;

#pragma warning(disable: 4100)
    //Raised when the mouse clicks on something.
    //The given Vector2i is the mouse position relative to this element's center.
    virtual void OnMouseClick(Vector2i mouse_centerOffset) { }
    //Raised when the mouse drags across the screen.
    //The given Vector2i instances are relative to this element's center.
    virtual void OnMouseDrag(Vector2i originalPos_centerOffset,
                             Vector2i currentPos_centerOffset) { }
    //Raised when the mouse releases over this element.
    //Ths given Vector2i is the current mouse position relative to this element's center.
    virtual void OnMouseRelease(Vector2i mouse_centerOffset) { }
#pragma warning(default: 4100)

    //Gets whether the given local-space position is inside this element's bounds.
    bool IsLocalInsideBounds(Vector2i pos) const;


protected:

    float CurrentTimeLerpSpeed = 0.0f;
   
    virtual void CustomUpdate(float elapsed) { }


    static DrawingQuad * GetQuad(void) { if (quad == 0) quad = new DrawingQuad(); return quad; }
    static void SetUpQuad(const RenderInfo & info, Vector2f pos,
                          Vector2f scale = Vector2f(1.0f, 1.0f), float rot = 0.0f);

private:

    static DrawingQuad * quad;
};