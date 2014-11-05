#pragma once

#include "GUITexture.h"


//This file defines a special GUIElement like GUIPanel but with auto-formatting of elements.
//This "GUIFormattedPanel" takes in a collection of "GUIFormatObject" pointers,
//    which describe the panel's layout.



//A position counter indicating where the next element should be placed,
//    and the current max width of the 
struct MovementData
{
public:
    //The position of the auto-formatting counter.
    Vector2f AutoPosCounter = Vector2f();
};


//A GUIElement plus a little spacing.
//If "MoveHorizontal" is true, this element takes up space horizontally and will push the counter horizontally.
//If "MoveVertical" is true, this element takes up space vertically and will push the counter vertically.
struct GUIFormatObject
{
public:

    GUIElementPtr Element;
    bool MoveHorizontal, MoveVertical;
    Vector2f SpaceAfter;

    GUIFormatObject(GUIElementPtr element = GUIElementPtr(0), bool moveHorz = false, bool moveVert = true,
                    Vector2f spaceAfter = Vector2f())
        : Element(element), SpaceAfter(spaceAfter), MoveHorizontal(moveHorz), MoveVertical(moveVert)
    { }


    //Positions this object based on the given struct and then updates the struct.
    void MoveObject(MovementData & data);
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
                      float timeLerpSpeed = 1.0f)
        : HorizontalBorder(horizontalBorder), VerticalBorder(verticalBorder),
          BackgroundTex(background), GUIElement(UniformDictionary(), timeLerpSpeed) { }


    void AddObject(const GUIFormatObject & toAdd);
    void AddRange(const std::vector<GUIFormatObject> & toInsert);
    void InsertObject(unsigned int index, const GUIFormatObject & toInsert);
    void InsertRange(unsigned int index, const std::vector<GUIFormatObject> & toInsert);
    void RemoveObject(unsigned int index);
    void ReplaceObject(unsigned int index, const GUIFormatObject & toAdd);

    bool ContainsElement(GUIElement* toFind);

    const std::vector<GUIFormatObject> & GetObjects(void) const { return objects; }
    const GUIFormatObject & GetFormatObject(unsigned int index) const { assert(index < objects.size()); return objects[index]; }
    GUIFormatObject & GetFormatObject(unsigned int index) { assert(index < objects.size()); return objects[index]; }


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

    Vector2f dimensions;

    //The objects to be arranged onto the panel.
    std::vector<GUIFormatObject> objects;

    //Re-calculates the position of the elements in this panel.
    void RePositionElements(void);
};