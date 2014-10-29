#pragma once

#include "../GUIElement.h"
#include "../TextRenderer.h"
#include "../../Materials/Data Nodes/ShaderGenerator.h"


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


    Material * RenderMat;

    
    //Starts out with no text (an empty string).
    GUILabel(const UniformDictionary & params,
             TextRenderer* _textRenderer = 0, TextRenderer::FontSlot textSlot = TextRenderer::FontSlot(),
             Material * material = 0, float timeSpeed = 1.0f,
             HorizontalOffsets _offsetH = HO_LEFT, VerticalOffsets _offsetV = VO_TOP)
        : offsetH(_offsetH), offsetV(_offsetV), textRenderer(_textRenderer),
          RenderMat(material), textRenderSlot(textSlot), text(textRenderer->GetString(textSlot)),
          GUIElement(params, timeSpeed)
    {
        if (!text.empty() && textRenderer != 0)
            dimensions = ToV2f(textRenderer->GetSlotBoundingSize(textSlot));
    }
    GUILabel(void) : GUIElement(UniformDictionary()) { }


    HorizontalOffsets GetOffsetHorz(void) const { return offsetH; }
    void SetOffsetHorz(HorizontalOffsets newOffsetH) { DidBoundsChange = true; offsetH = newOffsetH; }

    VerticalOffsets GetOffsetVert(void) const { return offsetV; }
    void SetOffsetVert(VerticalOffsets newOffsetV) { DidBoundsChange = true; offsetV = newOffsetV; }

    TextRenderer::FontSlot GetTextRenderSlot(void) const { return textRenderSlot; }
    void SetTextRenderSlot(TextRenderer::FontSlot newSlot);

    const TextRenderer* GetTextRenderer(void) const { return textRenderer; }
    TextRenderer* GetTextRenderer(void) { DidBoundsChange = true; return textRenderer; }

    const std::string & GetText(void) const { return text; }
    bool SetText(std::string newText);

    //Gets the size of the rendered text (without scaling).
    Vector2f GetTextSize(void) const { return dimensions; }


    virtual Vector2f GetPos(void) const override;
    virtual Box2D GetBounds(void) const override;

    virtual std::string Render(float elapsedTime, const RenderInfo & info) override;


private:

    //Gets the delta from this label's anchor point to the center of the text.
    Vector2f GetTextOffset(void) const;

    Vector2f dimensions;
    std::string text;

    TextRenderer::FontSlot textRenderSlot;
    TextRenderer* textRenderer;

    HorizontalOffsets offsetH;
    VerticalOffsets offsetV;
};