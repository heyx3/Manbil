#pragma once

#include "../GUIElement.h"
#include "GUILabel.h"
#include "GUITexture.h"


//A box the player can click on to bring up selection options.
//TODO: Add a highlight that sits on the item currently moused over.
//TODO: Make a more robust TextRenderer that can delete font slots (using a dictionary of slots instead of an array), then add the ability to insert/erase items from this element.
//TODO: Take in the constructor the horizontal (but not vertical) alighment of the text items.
class GUISelectionBox : public GUIElement
{
public:


    TextRenderer * TextRender;
    Material * BoxMat;
    MTexture2D * BoxTex;

    //Whether to display the options above or below this box.
    bool ExtendAbove;

    //Whether this box is currently open.
    bool IsExtended;

    //Whether to ignore any items that don't have any text.
    //This can be used to "delete" an item from the collection).
    bool DrawEmptyItems = false;

    //Raised when an option is selected.
    void(*OnOptionSelected)(GUISelectionBox * selector, const std::string & item, unsigned int itemIndex, void* pData) = 0;
    //Extra data to be passed to the 'OnOptionSelected' event.
    void* OnOptionSelected_pData = 0;


    //If this element is unable to create render slots for the items, "BoxMat" will be set to 0.
    //The number of items in the given vector is an upper bound on the number of items that can exist in this box.
    GUISelectionBox(TextRenderer* textRender, Material* selectionBoxMat,
                    MTexture2D * selectionBoxTex,
                    unsigned int fontID, Vector2u fontRenderTexSize,
                    const TextureSampleSettings2D & fontRenderSettings,
                    Material * itemTextMat,
                    const GUITexture & itemListBackground, 
                    std::vector<std::string> _items, unsigned int selected = 0,
                    bool extendAbove = true, float timeLerpSpeed = 1.0f);


    Vector2f GetScale(void) const { return scale; }

    Vector4f GetTextColor(void) const { return textColor; }
    void SetTextColor(Vector4f newCol);

    unsigned int GetNumbItems(void) const { return items.size(); }
    const std::string & GetItem(unsigned int index) const { return items[index]; }
    bool SetItem(unsigned int index, const std::string & newValue);

    virtual Vector2f GetCollisionCenter(void) const override { return center; }
    virtual Vector2f GetCollisionDimensions(void) const override;

    virtual void MoveElement(Vector2f moveAmount) override { center += moveAmount; }
    virtual void SetPosition(Vector2f newPos) override { center = newPos; }

    virtual void ScaleBy(Vector2f scaleAmount) override;
    virtual void SetScale(Vector2f newScale) override;

    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


    virtual void OnMouseClick(Vector2f relativeMousePos) override;
    virtual void OnMouseDrag(Vector2f oldRelativeMousePos,
                             Vector2f currentRelativeMousePos) override;
    virtual void OnMouseRelease(Vector2f relativeMousePos) override;


protected:


    virtual void CustomUpdate(float elapsed, Vector2f relativeMousePos) override;


    
private:

    GUITexture itemBackground;

    std::vector<std::string> items;
    std::vector<GUILabel> itemElements;

    unsigned int selectedItem;
    unsigned int itemFontID;

    Vector2f center;
    Vector2f scale = Vector2f(1, 1);
    Vector4f textColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
};