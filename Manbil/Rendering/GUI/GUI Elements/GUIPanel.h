#pragma once

#include "GUITexture.h"


//Contains an arbitrary collection of child GUI elements.
//TODO: Make a scrollable version of this and GUIFormattedPanel that renders these panels to a texture and has horizontal/vertical scroll bars.
class GUIPanel : public GUIElement
{
public:

    //Bounds and depth are automatically adjusted to fit this panel's bounds.
    //If it's invalid, it simply isn't drawn.
    GUITexture Background;


    GUIPanel(Vector2f borderSize = Vector2f(), float timeLerpSpeed = 1.0f);
    GUIPanel(GUITexture& background, Vector2f borderSize = Vector2f(), float timeLerpSpeed = 1.0f);


    //Adds the given element to this panel IF this panel doesn't have it already.
    void AddElement(GUIElementPtr element);
    //Returns whether or not the given element was a part of this panel before removing it.
    bool RemoveElement(GUIElementPtr element);
    //Gets whether this panel contains the given GUI element.
    bool ContainsElement(GUIElementPtr element) const;

    const std::vector<GUIElementPtr>& GetElements(void) const { return elements; }

    Vector2f GetBorderSize(void) const { return borderSize; }
    void SetBorderSize(Vector2f newSize) { DidBoundsChange = true; borderSize = newSize; }


    virtual Box2D GetBounds(void) const override;
    virtual bool GetDidBoundsChangeDeep(void) const override;
    virtual void ClearDidBoundsChangeDeep(void) override;

    virtual void ScaleBy(Vector2f scaleAmount) override;
    virtual void SetScale(Vector2f newScale) override;

    virtual void Render(float elapsedTime, const RenderInfo& info) override;

    virtual void OnMouseClick(Vector2f mouse_centerOffset) override;
    virtual void OnMouseDrag(Vector2f originalPos_centerOffset,
                             Vector2f mouse_centerOffset) override;
    virtual void OnMouseRelease(Vector2f mouse_centerOffset) override;


protected:

    virtual void CustomUpdate(float elapsedTime, Vector2f relativeMousePos) override;

    
private:

    //The extra horizontal/vertical border around the panel beyond the outermost elements.
    Vector2f borderSize;

    //The GUI elements inside this panel.
    std::vector<GUIElementPtr> elements;
};