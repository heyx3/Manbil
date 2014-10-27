#pragma once

#include "../GUIElement.h"


//Displays a texture. Can optionally be clicked.
class GUITexture : public GUIElement
{
public:

    Material * Mat;

    bool IsButton;

    //Only applicable if "IsButton" is true. Raised when this element is clicked.
    void(*OnClicked)(GUITexture * clicked, Vector2f localMouse, void* pData) = 0;
    //Only applicable if "IsButton" is true. Raised when the mouse releases after clicking this element.
    void(*OnReleased)(GUITexture * released, Vector2f localMouse, void* pData) = 0;
    //Only applicable if "IsButton" is true.
    void *OnClicked_pData = 0,
         *OnReleased_pData = 0;


    GUITexture(const UniformDictionary & params,
               MTexture2D * tex = 0, Material * mat = 0,
               bool isButton = false, float timeLerpSpeed = 1.0f)
        : tex(tex), Mat(mat), IsButton(isButton), GUIElement(params, timeLerpSpeed) { }
    GUITexture(void) : GUITexture(UniformDictionary()) { }


    //Gets whether this GUITexture is renderable (i.e. it has a material and texture).
    bool IsValid(void) const { return Mat != 0 && tex != 0; }

    //Gets this element's texture. Returns 0 if it doesn't have a texture.
    const MTexture2D* GetTex(void) const { return tex; }
    //Gets this element's texture. Returns 0 if it doesn't have a texture.
    //Assumes the texture bounds have changed because the returned texture isn't const.
    MTexture2D* GetTex(void) { DidBoundsChange = true; return tex; }

    //Sets this element's texture.
    void SetTex(MTexture2D* newTex) { DidBoundsChange = true; tex = newTex; }


    virtual Box2D GetBounds(void) const override;

    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;

    virtual void OnMouseClick(Vector2f mouse_centerOffset) override;
    virtual void OnMouseRelease(Vector2f mouse_centerOffset) override;

    
private:

    bool isBeingClicked;
    MTexture2D* tex;
};