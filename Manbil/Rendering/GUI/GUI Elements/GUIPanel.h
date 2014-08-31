#pragma once

#include "../GUIElement.h"


//Contains a collection of GUI elements.
class GUIPanel : public GUIElement
{
public:

    GUIPanel(const UniformDictionary & params, Vector2f _extents, float timeLerpSpeed = 1.0f)
        : extents(_extents), GUIElement(params, timeLerpSpeed) { }


    virtual Vector2f GetCollisionCenter(void) const override { return pos; }
    virtual Vector2f GetCollisionDimensions(void) const override { return extents; }

    virtual void MoveElement(Vector2f moveAmount) override { pos += moveAmount; }
    virtual void SetPosition(Vector2f newPos) override { pos = newPos; }

    virtual Vector2f GetScale(void) const override { return extents; }

    virtual void ScaleBy(Vector2f scaleAmount) override;
    virtual void SetScale(Vector2f newScale) override;


    //Adds the given element to this panel IF this panel doesn't have it already.
    void AddElement(GUIElement* element);
    //Returns whether or not the given element was a part of this panel before removing it.
    bool RemoveElement(GUIElement* element);
    //Gets whether this panel contains the given GUI element.
    bool ContainsElement(GUIElement* element) const;


    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


    virtual void OnMouseClick(Vector2f mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2f originalPos_centerOffset,
                             Vector2f mouse_centerOffset) override;
    virtual void OnMouseRelease(Vector2f mouse_centerOffset) override;


protected:

    virtual void CustomUpdate(float elapsedTime, Vector2f relativeMousePos) override;

    
private:

    //The GUI elements inside this panel.
    std::vector<GUIElement*> elements;
    Vector2f pos, extents;
};