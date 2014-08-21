#pragma once

#include "../GUIElement.h"


//Contains a collection of GUI panels.
//TODO: Should probably use Vector2f instead of Vector2i for more precision in positioning.
class GUIPanel : public GUIElement
{
public:

    //The GUI elements inside this panel.
    std::vector<Ptr> Elements;


    GUIPanel(Vector2i _extents, float timeLerpSpeed = 1.0f) : extents(_extents), GUIElement(timeLerpSpeed) { }


    virtual Vector2i GetCollisionCenter(void) const override { return pos; }
    virtual Vector2i GetCollisionDimensions(void) const override { return extents; }

    virtual void MoveElement(Vector2i moveAmount) override { pos += moveAmount; }
    virtual void SetPosition(Vector2i newPos) override { pos = newPos; }

    virtual void ScaleBy(Vector2f scaleAmount) override;
    virtual void SetScale(Vector2f newScale) override;


    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


    virtual void OnMouseClick(Vector2i mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2i originalPos_centerOffset,
                             Vector2i mouse_centerOffset) override;
    virtual void OnMouseRelease(Vector2i mouse_centerOffset) override;


protected:

    virtual void CustomUpdate(float elapsedTime, Vector2i relativeMousePos) override;

    
private:

    Vector2i pos;
    Vector2i extents;
};