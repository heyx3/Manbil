#pragma once

#include "GUITexture.h"


//This file defines a special GUIElement like GUIPanel but with auto-formatting of elements.
//This "GUIFormattedPanel" takes in a collection of "GUIFormatObject" pointers,
//    which describe the panel's layout.
//The panel is laid out as a series of vertical strips. Each strip contains GUI elements.
//There can occasionally be vertical breaks between elements.
//A horizontal break will push the next GUIElement to the right and at the top of a new strip.



//A position counter indicating where the next element should be placed,
//    and the current max width of the 
struct MovementData
{
public:
    //The new position of the auto-formatting counter.
    Vector2f AutoPosCounter = Vector2f();
    //The calculated width of this object (more accurately,
    //  the calculated distance from the auto-formatting counter's X and the right side of this object).
    float Width = 0.0f;
};


//Could be a GUIElement, a horizontal break, or a vertical break.
//This could be split into an abstract class and concrete implementations,
//   but it's more useful in this situation to refrain from hiding the details of each case.
struct GUIFormatObject
{
public:

    //The different types of format objects.
    enum ObjectTypes
    {
        OT_GUIELEMENT,
        OT_VERTBREAK,
        OT_HORZBREAK,
    };

    struct GUIElementType
    {
    public:
        float XOffset;
        GUIElementPtr Element;
        GUIElementType(GUIElementPtr element = 0, float xOffset = 0.0f)
            : Element(element), XOffset(xOffset) { }
    };
    struct HorzBreakType
    {
    public:
        //The extra X offset before the next vertical strip.
        float XOffset;
        HorzBreakType(float xOffset = 0.0f)
            : XOffset(xOffset) { }
    };


    ObjectTypes Type;

    //The vertical space after this object is put into the formatted panel.
    float SpaceAfter;

    GUIElementType GUIElementTypeData;
    HorzBreakType HorzBreakTypeData;


    //Creates a formatted GUIElement.
    GUIFormatObject(GUIElementType elementData, float spaceAfter = 0.0f)
        : Type(OT_GUIELEMENT), GUIElementTypeData(elementData), SpaceAfter(spaceAfter) { }

    //Creates a vertical break of the given size.
    GUIFormatObject(float spaceAfter = 0.0f) : Type(OT_VERTBREAK), SpaceAfter(spaceAfter) { }

    //Creates a horizontal break of the given dimensions.
    GUIFormatObject(HorzBreakType horzData, float spaceAfter = 0.0f)
        : Type(OT_HORZBREAK), HorzBreakTypeData(horzData), SpaceAfter(spaceAfter) { }


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


    GUIFormattedPanel(const UniformDictionary & params,
                      float horizontalBorder = 0.0f, float verticalBorder = 0.0f,
                      GUITexture background = GUITexture(UniformDictionary()),
                      float timeLerpSpeed = 1.0f)
        : HorizontalBorder(horizontalBorder), VerticalBorder(verticalBorder),
          extents(), BackgroundTex(background), GUIElement(params, timeLerpSpeed) { }


    void AddObject(const GUIFormatObject & toAdd);
    void RemoveObject(unsigned int index);
    bool ContainsElement(GUIElement* toFind);

    const std::vector<GUIFormatObject> & GetObjects(void) const { return objects; }
    const GUIFormatObject & GetFormatObject(unsigned int index) const { assert(index < objects.size()); return objects[index]; }
    GUIFormatObject & GetFormatObject(unsigned int index) { assert(index < objects.size()); return objects[index]; }

    //Re-calculates the position of the elements in this panel. Only needs to be called if a GUIFormatObject was manually modified.
    void RePositionElements(void);


    virtual Vector2f GetCollisionCenter(void) const override { return pos; }
    virtual Vector2f GetCollisionDimensions(void) const override { return extents; }

    virtual void MoveElement(Vector2f moveAmount) override { pos += moveAmount; }
    virtual void SetPosition(Vector2f newPos) override { pos = newPos; }

    virtual Vector2f GetScale(void) const override { return extents; }

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

    Vector2f pos, extents;

    //The objects to be arranged onto the panel.
    std::vector<GUIFormatObject> objects;
};