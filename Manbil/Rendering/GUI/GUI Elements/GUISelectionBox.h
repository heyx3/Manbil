#pragma once

#include "../GUIElement.h"
#include "GUILabel.h"
#include "GUITexture.h"


//A box the player can click on to bring up selection options.
//TODO: Add a highlight that sits on the item currently moused over.
//TODO: Make a more robust TextRenderer that can delete font slots (keep track of which slots were released and use those next), then add the ability to insert/erase items from this element.
class GUISelectionBox : public GUIElement
{
public:


    TextRenderer * TextRender;

    GUITexture BoxElement;

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


    //If this element is unable to create render slots for the items, "BoxElement.Mat" will be set to 0.
    //The number of items in the given vector is an upper bound on the number of items that can exist in this box.
    GUISelectionBox(const UniformDictionary & params,
                    TextRenderer* textRender, const GUITexture & selectionBoxCopy,
                    unsigned int fontID, Vector2u fontRenderTexSize,
                    const TextureSampleSettings2D & fontRenderSettings,
                    Material * itemTextMat, GUILabel::HorizontalOffsets textOffset,
                    const GUITexture & itemListBackground, 
                    const std::vector<std::string> & _items, unsigned int selected = 0,
                    bool extendAbove = true, float timeLerpSpeed = 1.0f);
    GUISelectionBox(void) : GUIElement(UniformDictionary()), itemBackground(UniformDictionary()) { }

    //Don't allow copying of this class since it creates TextRenderer slots in the constructor.
    //TODO: Figure out a way around this; these classes should be copyable. Maybe make a static method that does those allocations, while the constructor takes in the slots to use.
    GUISelectionBox(const GUISelectionBox & cpy) = delete;


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