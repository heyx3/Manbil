#pragma once

#include "../GUIElement.h"


//Contains an arbitrary collection of child GUI elements.
//TODO: If this panel is marked as "scrollable", render its elements to a texture and then render that texture, so that the elements never extend outside this panel's bounds. Also have a private GUISlider that acts as the scroll bar.
class GUIPanel : public GUIElement
{
public:

    GUIPanel(float timeLerpSpeed = 1.0f) : GUIElement(UniformDictionary(), timeLerpSpeed) { }


    //Adds the given element to this panel IF this panel doesn't have it already.
    void AddElement(GUIElementPtr element);
    //Returns whether or not the given element was a part of this panel before removing it.
    bool RemoveElement(GUIElementPtr element);
    //Gets whether this panel contains the given GUI element.
    bool ContainsElement(GUIElementPtr element) const;

    const std::vector<GUIElementPtr> & GetElements(void) const { return elements; }


    virtual Box2D GetBounds(void) const override;
    virtual bool GetDidBoundsChangeDeep(void) const override;
    virtual void ClearDidBoundsChangeDeep(void) override;

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

    //The GUI elements inside this panel.
    std::vector<GUIElementPtr> elements;
};