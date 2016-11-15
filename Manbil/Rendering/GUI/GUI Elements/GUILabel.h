#pragma once

#include "../GUIElement.h"
#include "../TextRenderer.h"
#include "../../Data Nodes/ShaderGenerator.h"


//Represents a simple piece of text.
class GUILabel : public GUIElement
{
public:

    enum HorizontalOffsets
    {
        HO_LEFT,
        HO_CENTER,
        HO_RIGHT,
    };
    enum VerticalOffsets
    {
        VO_TOP,
        VO_CENTER,
        VO_BOTTOM,
    };


    Material* RenderMat;

    //If set to true, then when this label is deleted, it will delete the text slot it uses.
    //Be careful with this! Only use it if you heap-allocated this instance; otherwise it will delete
    //    the slot when being copied.
    bool DeleteSlotWhenDeleted = false;

    
    //Starts out with no text (an empty string).
    GUILabel(const UniformDictionary& params,
             TextRenderer* _textRenderer = 0,
             TextRenderer::FontSlot textSlot = TextRenderer::FontSlot(),
             Material* material = 0, float timeSpeed = 1.0f,
             HorizontalOffsets _offsetH = HO_LEFT, VerticalOffsets _offsetV = VO_TOP);
    GUILabel(void) : GUIElement(UniformDictionary()) { }

    ~GUILabel(void) { if (DeleteSlotWhenDeleted) textRenderer->DeleteTextRenderSlot(textRenderSlot); }


    HorizontalOffsets GetOffsetHorz(void) const { return offsetH; }
    void SetOffsetHorz(HorizontalOffsets newOffsetH) { DidBoundsChange = true; offsetH = newOffsetH; }

    VerticalOffsets GetOffsetVert(void) const { return offsetV; }
    void SetOffsetVert(VerticalOffsets newOffsetV) { DidBoundsChange = true; offsetV = newOffsetV; }

    TextRenderer::FontSlot GetTextRenderSlot(void) const { return textRenderSlot; }
    void SetTextRenderSlot(TextRenderer::FontSlot newSlot);
    
    //Gets this element's text renderer. Returns 0 if it doesn't have one.
    const TextRenderer* GetTextRenderer(void) const { return textRenderer; }

    const std::string& GetText(void) const { return text; }
    bool SetText(std::string newText);

    //Gets the size of the rendered text (without scaling).
    Vector2f GetTextSize(void) const { return dimensions; }


    virtual Box2D GetBounds(void) const override;

    virtual void Render(float elapsedTime, const RenderInfo& info) override;


private:

    //Gets the delta from this label's anchor point to the center of the text.
    Vector2f GetAnchorToTextCenter(void) const;
    //Gets the delta from this label's anchor point to the center of the rendered text texture.
    Vector2f GetAnchorToRenderCenter(void) const;

    //The un-scaled size of the rendered text.
    Vector2f dimensions;
    std::string text;

    TextRenderer::FontSlot textRenderSlot;
    TextRenderer* textRenderer;

    HorizontalOffsets offsetH;
    VerticalOffsets offsetV;
};