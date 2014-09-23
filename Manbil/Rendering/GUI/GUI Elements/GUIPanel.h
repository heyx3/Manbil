#pragma once

#include "../GUIElement.h"


//Contains a collection of GUI elements.
//TODO: If this panel is marked as "scrollable", render its elements to a texture and then render that texture, so that the elements never extend outside this panel's bounds. Also have a private GUISlider that acts as the scroll bar.
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
    void AddElement(GUIElementPtr element);
    //Returns whether or not the given element was a part of this panel before removing it.
    bool RemoveElement(GUIElementPtr element);
    //Gets whether this panel contains the given GUI element.
    bool ContainsElement(GUIElementPtr element) const;

    //Recalculates the position/extents of this panel.
    //Only needs to be called externally if a child GUIElement was modified.
    void RecalcPosition(void);

    const std::vector<GUIElementPtr> & GetElements(void) const { return elements; }
    std::vector<GUIElementPtr> & GetElements(void) { return elements; }


    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


    virtual void OnMouseClick(Vector2f mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2f originalPos_centerOffset,
                             Vector2f mouse_centerOffset) override;
    virtual void OnMouseRelease(Vector2f mouse_centerOffset) override;


protected:

    virtual void CustomUpdate(float elapsedTime, Vector2f relativeMousePos) override;

    
private:

    //The GUI elements inside this panel.
    std::vector<GUIElementPtr> elements;
    Vector2f pos, extents;
};