#pragma once

#include "../GUIElement.h"


//Displays a texture. Can optionally be clicked.
class GUITexture : public GUIElement
{
public:

    Material * Mat;

    MTexture2D * Tex;
    Vector2f Scale;

    bool IsButton;

    //Only applicable if "IsButton" is true. Raised when this element is clicked.
    void(*OnClicked)(GUITexture * clicked, Vector2f localMouse, void* pData) = 0;
    //Only applicable if "IsButton" is true. Raised when the mouse releases after clicking this element.
    void(*OnReleased)(GUITexture * released, Vector2f localMouse, void* pData) = 0;
    //Only applicable if "IsButton" is true.
    void *OnClicked_pData = 0,
         *OnReleased_pData = 0;


    virtual Vector2f GetCollisionCenter(void) const override { return center; }
    virtual Vector2f GetCollisionDimensions(void) const override;

    virtual void MoveElement(Vector2f moveAmount) override { center += moveAmount; }
    virtual void SetPosition(Vector2f newPos) override { center = newPos; }

    virtual void ScaleBy(Vector2f scaleAmount) override { Scale.MultiplyComponents(scaleAmount); }
    virtual void SetScale(Vector2f newScale) override { Scale = newScale; }


    GUITexture(MTexture2D * tex = 0, Material * mat = 0,
               bool isButton = false, float timeLerpSpeed = 1.0f)
        : Tex(tex), Mat(mat), IsButton(isButton), GUIElement(timeLerpSpeed), Scale(1.0f, 1.0f) { }


    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;

    virtual void OnMouseClick(Vector2f mouse_centerOffset) override;
    virtual void OnMouseRelease(Vector2f mouse_centerOffset) override;

    
private:

    Vector2f center;
    bool isBeingClicked;
};