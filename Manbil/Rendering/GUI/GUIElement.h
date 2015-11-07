#pragma once

#include "../Primitives/DrawingQuad.h"
#include "../../Math/Shapes/Boxes.h"


//TODO: Decouple "mouse pointer" and "is selected", so that the element can optionally have NO mouse pos input.
//TODO: Remove "mouse click" from "update" cycle and create some kind of generic "Activated()" event for mouse button/keyboard/gamepad/etc.
//PRIORITY: Fix this system so that scale is relative to parent like everything else, and the position offset scales in "RenderChild()".

//Represents a single element in a UI.
//Keeps track of whether its size/position has changed since the last frame.
//Provides support for the concept of "child" elements that this element contains.
//Note that mouse events are assumed to be raised for all GUI elements in a scene,
//    not just the ones that are actually touching the mouse.
class GUIElement
{
public:

    UniformDictionary Params;
    float TimeLerpSpeed;
    
    float Depth = 0.0f;

    //Whether this element's position, size, scale, etc. changed since the last Update() call.
    //Note that this only reflects this element and NOT any of its children;
    //    call "GetDidBoundsChangeDeep()" to get whether this element or its children changed.
    //Resets to "false" at the very beginning of every Update() call.
    bool DidBoundsChange = false;

    //Raised every update step, before anything is updated.
    //"pData" is the value of this instance's "OnUpdate_Data" field.
    void(*OnUpdate)(GUIElement* thisEl, float elapsed, Vector2f localMouse, void* pData) = 0;
    void* OnUpdate_Data = 0;


    GUIElement(const UniformDictionary& params, float timeLerpSpeed = 1.0f)
        : TimeLerpSpeed(timeLerpSpeed), scale(1.0f, 1.0f) { Params = params; }
    virtual ~GUIElement(void) { }


    bool UsesTimeLerp(void) const;

    float GetTimeLerp(void) const;
    void SetTimeLerp(float newVal);

    //Returns { 1.0f, 1.0f, 1.0f, 1.0f } if this element doesn't have a color entry in its params.
    Vector4f GetColor(void) const;
    void SetColor(Vector4f newCol);

    bool IsMousedOver(void) const { return isMousedOver; }


    //Calculates whether this element's bounds have changed since the last Update() call,
    //    including any child elements changing.
    //Default behavior: just returns "DidBoundsChange".
    //TODO: Why is this always returning "true"??
    virtual bool GetDidBoundsChangeDeep(void) const { return true; }
    //Resets this element's "DidBoundsChange" and any childrens' "DidBoundsChange".
    //Default behavior: just sets "DidBoundsChange" to false.
    virtual void ClearDidBoundsChangeDeep(void) { DidBoundsChange = false; }

    //Gets the location of the "anchor" of this element -- generally the center of its collision box.
    //Default behavior: just returns the position.
    virtual Vector2f GetPos(void) const { return pos; }
    //Gets the collision box based on this element's position and dimensions.
    virtual Box2D GetBounds(void) const = 0;

    //Moves this element by the given amount.
    //Default behavior: enables "DidBoundsChange" and adds the given amount to the position.
    virtual void MoveElement(Vector2f moveAmount) { DidBoundsChange = true; pos += moveAmount; }
    //Sets this element's position.
    //Default behavior: enables "DidBoundsChange" and adds the given amount to the position.
    virtual void SetPosition(Vector2f newPos) { DidBoundsChange = true; pos = newPos; }

    //Gets this element's scale.
    Vector2f GetScale(void) const { return scale; }
    //Scales this element by the given amount.
    //Default behavior: enables "DidBoundsChange" and scales this element.
    virtual void ScaleBy(Vector2f scaleAmount);
    //Sets this element's scale to the given amount (may be approximate).
    //Default behavior: enables "DidBoundsChange" and sets this element's scale.
    virtual void SetScale(Vector2f newScale) { DidBoundsChange = true; scale = newScale; }

    //Sets this element to occupy the given bounds, by calculating delta pos and scale to move
    //    from the current bounds to the given bounds.
    void SetBounds(Box2D newBounds);


    //Takes in the mouse position relative to this element's center.
    void Update(float elapsedTime, Vector2f relativeMousePos);

    //Returns an error message, or the empty string if everything went fine.
    virtual void Render(float elapsedTime, const RenderInfo& info) = 0;


#pragma warning(disable: 4100)
    //Raised when the mouse clicks on something.
    //The given Vector2i is the mouse position relative to this element's center.
    virtual void OnMouseClick(Vector2f relativeMousePos) { }
    //Raised when the mouse drags across the screen.
    //The given Vector2i instances are relative to this element's center.
    virtual void OnMouseDrag(Vector2f originalRelativeMousePos,
                             Vector2f currentRelativeMousePos) { }
    //Raised when the mouse releases over this element.
    //Ths given Vector2i is the current mouse position relative to this element's center.
    virtual void OnMouseRelease(Vector2f relativeMousePos) { }
#pragma warning(default: 4100)


protected:

    float CurrentTimeLerpSpeed = 0.0f;
   

#pragma warning(disable: 4100)
    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) { }
#pragma warning(default: 4100)

    //Renders the given child of this element. Assumes that the child's position, depth, and color
    //    are all relative to this "parent" GUIElement.
    //Does not affect the child's "DidBoundsChange" field.
    void RenderChild(GUIElement* child, float elapsedTime, const RenderInfo& info) const;

    //Gets the quad used to draw all GUIElements.
    static DrawingQuad* GetQuad(void) { return DrawingQuad::GetInstance(); }
    
    //Sets up the drawing quad with the given bounds and depth.
    static void SetUpQuad(const Box2D& bounds, float depth);
    //Sets up the drawing quad with this element's bounds and depth.
    void SetUpQuad(void) const;


private:

    Vector2f pos, scale;
    bool isMousedOver = false;
};


typedef std::shared_ptr<GUIElement> GUIElementPtr;