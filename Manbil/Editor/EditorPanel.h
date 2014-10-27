#pragma once

#include "EditorObject.h"
#include "../Rendering/GUI/GUI Elements/GUIFormattedPanel.h"


//Defines a panel for editing primitive values (int, float, bool, string, etc).
//These panels are defined by a list of heap-allocated "EditorObject" instances.



//An auto-formatted editor for some kind of basic data.
class EditorPanel : public GUIElement
{
public:

    EditorMaterialSet & MaterialSet;


    EditorPanel(EditorMaterialSet & set, float horizontalBorder, float verticalBorder)
        : MaterialSet(set), panel(set.StaticMatGreyParams, horizontalBorder, verticalBorder,
                                  GUITexture(set.GetStaticMatParams(&set.PanelBackgroundTex),
                                             &set.PanelBackgroundTex,
                                             set.GetStaticMaterial(&set.PanelBackgroundTex))),
          GUIElement(UniformDictionary()) { }
    

    const GUIFormattedPanel & GetPanel(void) const { return panel; }
    void RePositionElements(void) { panel.RePositionElements(); }


    //Initializes the given object and then adds it to this editor panel.
    //Returns an error message when initializing this object, or the empty string if everything is fine.
    std::string AddObject(EditorObjectPtr toAdd) { return AddObject(toAdd, editorObjects.size()); }
    //Initializes the given object and then adds it to this editor panel at the given location.
    //Returns an error message when initializing this object, or the empty string if everything is fine.
    std::string AddObject(EditorObjectPtr toAdd, unsigned int index);

    //Initializes the given objects and then adds them to this editor panel.
    //Returns an error message when initializing these objects, or the empty string if everything is fine.
    std::string AddObjects(const std::vector<EditorObjectPtr> & toAdd) { return AddObjects(toAdd, editorObjects.size()); }
    //Initializes the given objects and then adds them to this editor panel.
    //Returns an error message when initializing these objects, or the empty string if everything is fine.
    std::string AddObjects(const std::vector<EditorObjectPtr> & toAdd, unsigned int startIndex);

    //Removes the given object from this editor panel if it exists.
    //Returns whether or not it existed in this panel.
    bool RemoveObject(EditorObjectPtr toRemove);


    const std::vector<EditorObjectPtr> & GetObjects(void) const { return editorObjects; }


    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


    //Most of the overridden GUIElement functions just pass through to the GUIFormattedPanel.
    virtual Vector2f GetCollisionCenter(void) const override { return panel.GetCollisionCenter(); }
    virtual Vector2f GetCollisionDimensions(void) const override { return panel.GetCollisionDimensions(); }
    virtual void MoveElement(Vector2f moveAmount) override { panel.MoveElement(moveAmount); }
    virtual void SetPosition(Vector2f newPos) override { panel.SetPosition(newPos); }
    virtual Vector2f GetScale(void) const override { return panel.GetScale(); }
    virtual void ScaleBy(Vector2f scaleAmount) override { panel.ScaleBy(scaleAmount); }
    virtual void SetScale(Vector2f newScale) override { panel.SetScale(newScale); }
    virtual void OnMouseClick(Vector2f relativeMousePos) { panel.OnMouseClick(relativeMousePos); }
    virtual void OnMouseDrag(Vector2f originalMPos, Vector2f currentMPos) { panel.OnMouseDrag(originalMPos, currentMPos); }
    virtual void OnMouseRelease(Vector2f relativeMousePos) override { panel.OnMouseRelease(relativeMousePos); }


protected:

    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) override;


private:

    GUIFormattedPanel panel;
    std::vector<EditorObjectPtr> editorObjects;
};