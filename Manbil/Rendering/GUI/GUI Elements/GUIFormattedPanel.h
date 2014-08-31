#pragma once

#include "../GUIElement.h"


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
        float SpaceAfter, XOffset;
        GUIElement* Element;
        GUIElementType(GUIElement* element = 0, float spaceAfter = 0.0f, float xOffset = 0.0f)
            : Element(element), SpaceAfter(spaceAfter), XOffset(xOffset) { }
    };
    struct VertBreakType
    {
    public:
        float SpaceAfter;
        VertBreakType(float spaceAfter = 0.0f) : SpaceAfter(spaceAfter) { }
    };
    struct HorzBreakType
    {
    public:
        //The space between the top of the panel and the beginning of the next vertical strip.
        float VerticalBorder;
        //The extra X offset before the next vertical strip.
        float XOffset;
        HorzBreakType(float vertBorder = 0.0f, float xOffset = 0.0f)
            : VerticalBorder(vertBorder), XOffset(xOffset) { }
    };


    ObjectTypes Type;

    GUIElementType GUIElementTypeData;
    VertBreakType VertBreakTypeData;
    HorzBreakType HorzBreakTypeData;


    GUIFormatObject(void) : Type(OT_VERTBREAK) { }
    GUIFormatObject(GUIElementType elementData) : Type(OT_GUIELEMENT), GUIElementTypeData(elementData) { }
    GUIFormatObject(VertBreakType vertData) : Type(OT_VERTBREAK), VertBreakTypeData(vertData) { }
    GUIFormatObject(HorzBreakType horzData) : Type(OT_HORZBREAK), HorzBreakTypeData(horzData) { }


    //Positions this object based on the current auto pos counter and updates the given struct.
    virtual void MoveObject(MovementData & data)
    {
        GUIElement* el;
        float spaceAfter, xOffset;
        Vector2f dims, min;

        switch (Type)
        {
            case OT_GUIELEMENT:

                el = GUIElementTypeData.Element;
                spaceAfter = GUIElementTypeData.SpaceAfter;
                xOffset = GUIElementTypeData.XOffset;

                dims = el->GetCollisionDimensions();
                min = Vector2f(data.AutoPosCounter.x + xOffset, data.AutoPosCounter.y);
                el->SetBounds(min, Vector2f(min.x + dims.x, min.y + dims.y));

                data.AutoPosCounter.y += dims.y + spaceAfter;
                data.Width = BasicMath::Max(data.Width, xOffset + dims.x);

                break;

            case OT_VERTBREAK:
                data.AutoPosCounter = Vector2f(data.AutoPosCounter.x,
                                               data.AutoPosCounter.y + VertBreakTypeData.SpaceAfter);
                break;
            case OT_HORZBREAK:
                data.AutoPosCounter.x += data.Width + HorzBreakTypeData.XOffset;
                data.AutoPosCounter.y = HorzBreakTypeData.VerticalBorder;
                data.Width = 0.0f;
                break;


            default:
                assert(false);
                break;
        }
    }
};


//Contains a collection of GUI elements.
//The elements are auto-formatted to look nice.
class GUIFormattedPanel : public GUIElement
{
public:

    //The objects to be arranged onto the panel.
    std::vector<GUIFormatObject> Objects;


    GUIFormattedPanel(const UniformDictionary & params, Vector2f _extents, float timeLerpSpeed = 1.0f)
        : extents(_extents), GUIElement(params, timeLerpSpeed)
    { }


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
};