#pragma once

#include <string>
#include "../Rendering/GUI/GUIElement.h"
#include "EditorMaterialSet.h"


//A (usually modifiable) piece of data to be displayed in the editor.
struct EditorObject
{
public:


    //Data for the description label next to the editable object.
    struct DescriptionData
    {
    public:
        std::string Text;
        bool IsOnLeft;
        float Spacing;
        unsigned int TextRenderWidth;

        DescriptionData(std::string description = "", bool isDescOnLeft = true,
                        float descSpacing = 10.0f, unsigned int descRenderWidth = 1024)
            : Text(description), IsOnLeft(isDescOnLeft), Spacing(descSpacing),
              TextRenderWidth(descRenderWidth) { }
    };
    DescriptionData DescriptionLabel;

    //The amount of space after this object.
    Vector2f Offset;


    //If an empty description is passed, this editor object should not even create a label.
    EditorObject(const DescriptionData& descriptionLabel, Vector2f spaceAfter = Vector2f())
        : activeGUIElement(0), Offset(spaceAfter), DescriptionLabel(descriptionLabel) { }

    virtual ~EditorObject(void) { }


    //Gets the currently-active GUIElement.
    GUIElementPtr GetActiveGUIElement(void) const { return activeGUIElement; }
    //Gets whether to move horizontally in the editor past this object.
    //Default behavior: returns false.
    virtual bool GetMoveHorizontally(void) const { return false; }
    //Gets whether to move vertically in the editor past this object.
    //Default behavior: returns true.
    virtual bool GetMoveVertically(void) const { return true; }

    //Initializes the GUIElement for representing/editing this data.
    //Returns an error message, or the empty string if everything went fine.
    //Takes in the content to use for rendering.
    virtual std::string InitGUIElement(EditorMaterialSet& materialSet) = 0;
    
#pragma warning(disable: 4100)
    //Updates this object and its element's properties.
    //Does NOT need to call Update() for this object's active GUI element.
    //Returns whether the active GUI element was moved, changed, or resized at all
    //   (i.e. whether the formatted panel has to rebuild its layout).
    //Default behavior: do nothing, return false.
    virtual bool Update(float elapsed, Vector2f panelRelativeMousePos) { return false; }
#pragma warning(default: 4100)

protected:

    GUIElementPtr activeGUIElement;

    //Puts the description next to the given GUIElement in a GUIFormattedPanel and returns the panel.
    //If there was an error, sets the given error message string and returns a pointer to 0.
    GUIElementPtr AddDescription(EditorMaterialSet& materialSet, GUIElementPtr element,
                                 std::string& errorMsg) const;
};


typedef std::shared_ptr<EditorObject> EditorObjectPtr;