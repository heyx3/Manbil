#pragma once

#include "EditorObject.h"
#include "../Rendering/GUI/GUI Elements/GUIFormattedPanel.h"


//TODO: Create a new system on top of this one that implements a DataReader/DataWriter to create an editor panel for serialized data.


//Provides a high-level method for creating editor panels for some data.
class EditorPanel : public GUIElement
{
public:

    EditorMaterialSet& MaterialSet;


    EditorPanel(EditorMaterialSet& set, float horizontalBorder, float verticalBorder);
    

    const GUIFormattedPanel& GetPanel(void) const { return panel; }


    //Initializes the given object and then adds it to this editor panel.
    //Returns an error message when initializing this object, or the empty string if everything is fine.
    std::string AddObject(EditorObjectPtr toAdd) { return AddObject(toAdd, editorObjects.size()); }
    //Initializes the given object and then adds it to this editor panel at the given location.
    //Returns an error message when initializing this object, or the empty string if everything is fine.
    std::string AddObject(EditorObjectPtr toAdd, unsigned int index);

    //Initializes the given objects and then adds them to this editor panel.
    //Returns an error message when initializing these objects, or the empty string if everything is fine.
    std::string AddObjects(const std::vector<EditorObjectPtr>& toAdd);
    //Initializes the given objects and then adds them to this editor panel.
    //Returns an error message when initializing these objects, or the empty string if everything is fine.
    std::string AddObjects(const std::vector<EditorObjectPtr>& toAdd, unsigned int startIndex);

    //Removes the given object from this editor panel if it exists.
    //Returns whether or not it existed in this panel.
    bool RemoveObject(EditorObjectPtr toRemove);

    const std::vector<EditorObjectPtr>& GetObjects(void) const { return editorObjects; }

    virtual bool GetDidBoundsChangeDeep(void) const override;
    virtual void ClearDidBoundsChangeDeep(void) override;

    virtual void Render(float elapsedTime, const RenderInfo& info) override;


    //Most of these GUIElement functions just pass through to the encapsulated GUIFormattedPanel.
    virtual Vector2f GetPos(void) const override;
    virtual Box2D GetBounds(void) const override;
    virtual void MoveElement(Vector2f moveAmount) override;
    virtual void SetPosition(Vector2f newPos) override;
    virtual void ScaleBy(Vector2f scaleAmount) override;
    virtual void SetScale(Vector2f newScale) override;
    virtual void OnMouseClick(Vector2f relativeMousePos) override;
    virtual void OnMouseDrag(Vector2f originalMPos, Vector2f currentMPos) override;
    virtual void OnMouseRelease(Vector2f relativeMousePos) override;


protected:

    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) override;


private:

    GUIFormattedPanel panel;
    std::vector<EditorObjectPtr> editorObjects;
};