#pragma once

#include "../GUIElement.h"


//Contains a collection of GUI panels.
class GUIPanel : public GUIElement
{
public:

    //The GUI elements inside this panel.
    std::vector<GUIElement*> Elements;


    GUIPanel(const UniformDictionary & params, Vector2f _extents, float timeLerpSpeed = 1.0f)
        : extents(_extents), GUIElement(params, timeLerpSpeed) { }


    virtual Vector2f GetCollisionCenter(void) const override { return pos; }
    virtual Vector2f GetCollisionDimensions(void) const override { return extents; }

    virtual void MoveElement(Vector2f moveAmount) override { pos += moveAmount; }
    virtual void SetPosition(Vector2f newPos) override { pos = newPos; }

    virtual Vector2f GetScale(void) const override { return extents; }

    virtual void ScaleBy(Vector2f scaleAmount) override;
    virtual void SetScale(Vector2f newScale) override;


    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


    virtual void OnMouseClick(Vector2f mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2f originalPos_centerOffset,
                             Vector2f mouse_centerOffset) override;
    virtual void OnMouseRelease(Vector2f mouse_centerOffset) override;


protected:

    virtual void CustomUpdate(float elapsedTime, Vector2f relativeMousePos) override;

    
private:

    Vector2f pos, extents;
};