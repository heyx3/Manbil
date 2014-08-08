#pragma once

#include "../GUIElement.h"


//Contains a collection of GUI panels.
class GUIPanel : public GUIElement
{
public:

    //The GUI elements inside this panel.
    std::vector<Ptr> Elements;


    virtual Vector2i GetCollisionCenter(void) const override { return pos; }
    virtual Vector2i GetCollisionDimensions(void) const override { return extents; }

    virtual void MoveElement(Vector2i moveAmount) override { pos += moveAmount; }
    virtual void SetPosition(Vector2i newPos) override { pos = newPos; }

    virtual void ScaleBy(Vector2f scaleAmount) override;


    virtual void Update(float elapsedTime) override;
    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


    virtual void OnMouseClick(Vector2i mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2i originalPos_centerOffset,
                             Vector2i mouse_centerOffset) override;
    virtual void OnMouseRelease(Vector2i mouse_centerOffset) override;

    
private:

    Vector2i pos;
    Vector2i extents;
};