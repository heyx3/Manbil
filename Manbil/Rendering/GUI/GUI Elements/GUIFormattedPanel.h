#pragma once

#include "GUITexture.h"


//This file defines a special GUIElement similar to GUIPanel but with automated layout of elements.
//This "GUIFormattedPanel" takes in a collection of "GUIFormatObject" instances,
//    which describes an element and how it should be laid out.


//A counter indicating where the next element in the layout should be placed.
//Used in the GUIFormattedPanel's algorithm for laying out a scene.
struct MovementData;

//A GUIElement plus some layout info.
struct GUIFormatObject
{
public:

    GUIElementPtr Element;

    //Extra space after this element.
    Vector2f SpaceAfter;

    //If true, this element takes up space horizontally and will push the layout counter horizontally.
    bool MoveHorizontal;
    //If true, this element takes up space vertically and will push the layout counter vertically.
    bool MoveVertical;


    GUIFormatObject(GUIElementPtr element = GUIElementPtr(0),
                    bool moveHorz = false, bool moveVert = true,
                    Vector2f spaceAfter = Vector2f());


    //Positions this object based on the given data and then updates the data.
    void MoveObject(MovementData& data);
};


//Contains a collection of GUI elements.
//The elements are auto-formatted to look nice.
class GUIFormattedPanel : public GUIElement
{
public:

    //Optional background texture (set the texture or material to 0 to disable it).
    GUITexture BackgroundTex;

    //The border around the items.
    float HorizontalBorder, VerticalBorder;


    GUIFormattedPanel(float horizontalBorder = 0.0f, float verticalBorder = 0.0f,
                      GUITexture background = GUITexture(UniformDictionary()),
                      float timeLerpSpeed = 1.0f);


    void AddObject(const GUIFormatObject& toAdd);
    void AddRange(const std::vector<GUIFormatObject>& toInsert);
    void InsertObject(unsigned int index, const GUIFormatObject& toInsert);
    void InsertRange(unsigned int index, const std::vector<GUIFormatObject>& toInsert);
    void RemoveObject(unsigned int index);
    void ReplaceObject(unsigned int index, const GUIFormatObject& toAdd);

    bool ContainsElement(GUIElement* toFind);

    const std::vector<GUIFormatObject>& GetObjects(void) const { return objects; }
    const GUIFormatObject& GetFormatObject(unsigned int index) const;
    GUIFormatObject& GetFormatObject(unsigned int index);


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

    Vector2f dimensions;

    //The objects to be arranged onto the panel.
    std::vector<GUIFormatObject> objects;

    //Re-calculates the position of the elements in this panel.
    void RePositionElements(void);
};