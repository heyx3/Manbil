#pragma once

#include "../Helper Classes/DrawingQuad.h"
#include "GUIMaterials.h"


//Represents a single element in a UI.
//Note that mouse events should be raised for all GUI elements, not just ones that are touching the mouse.
//TODO: Add a protected "RenderChild" method that handles all the relative positioning/depth/color stuff for a given child element and then renders it.
//TODO: Keep a static reference to the currently-moused-over object, and if another object is being moused over, compare their depths to see which one stays moused over.
class GUIElement
{
public:

    UniformDictionary Params;
    float TimeLerpSpeed;
    
    float Depth = 0.0f;

    //Raised every update step, before anything is updated.
    //"pData" is the value of this instance's "OnUpdate_Data" field.
    void(*OnUpdate)(GUIElement * thisEl, void* pData) = 0;
    void* OnUpdate_Data = 0;


    bool UsesTimeLerp(void) const { return Params.FloatUniforms.find(GUIMaterials::DynamicQuadDraw_TimeLerp) != Params.FloatUniforms.end(); }

    float GetTimeLerp(void) const { return Params.FloatUniforms.find(GUIMaterials::DynamicQuadDraw_TimeLerp)->second.Value[0]; }
    void SetTimeLerp(float newVal) { Params.FloatUniforms[GUIMaterials::DynamicQuadDraw_TimeLerp].SetValue(newVal); }

    Vector4f GetColor(void) const { return *(Vector4f*)&Params.FloatUniforms.find(GUIMaterials::QuadDraw_Color)->second.Value; }
    void SetColor(Vector4f newCol) { Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(newCol); }

    bool IsMousedOver(void) const { return isMousedOver; }


    GUIElement(const UniformDictionary & params, float timeLerpSpeed = 1.0f) : TimeLerpSpeed(timeLerpSpeed) { Params = params; }
    virtual ~GUIElement(void) { }


    //The center of this element's collision box.
    virtual Vector2f GetCollisionCenter(void) const = 0;
    //The size of this element's collision box.
    virtual Vector2f GetCollisionDimensions(void) const = 0;

    //Moves this element by the given amount.
    virtual void MoveElement(Vector2f moveAmount) = 0;
    //Sets this element's position.
    virtual void SetPosition(Vector2f newPos) = 0;

    //Gets this element's scale.
    virtual Vector2f GetScale(void) const = 0;
    //Scales this element by the given amount (may be approximate).
    virtual void ScaleBy(Vector2f scaleAmount) = 0;
    //Sets this element's scale to the given amount (may be approximate).
    virtual void SetScale(Vector2f newScale) = 0;

    //Sets this elements' center and scale to fit the given bounds.
    void SetBounds(Vector2f min, Vector2f max);


    //Takes in the mouse position relative to this element's center.
    void Update(float elapsedTime, Vector2f mouse_centerOffset);

    //Returns an error message, or the empty string if everything went fine.
    virtual std::string Render(float elapsedTime, const RenderInfo & info) = 0;


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
    virtual void OnMouseRelease(Vector2f mouse_centerOffset) { }
#pragma warning(default: 4100)


    //Gets whether the given local-space position is inside this element's bounds.
    bool IsLocalInsideBounds(Vector2f pos) const;


protected:

    float CurrentTimeLerpSpeed = 0.0f;
   
    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) { }


    static DrawingQuad * GetQuad(void) { if (quad == 0) quad = new DrawingQuad(); return quad; }
    static void SetUpQuad(const RenderInfo & info, Vector2f pos, float depth,
                          Vector2f scale = Vector2f(1.0f, 1.0f), float rot = 0.0f);

private:

    static DrawingQuad * quad;

    bool isMousedOver = false;
};