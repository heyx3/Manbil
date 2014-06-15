#pragma once

#include "TextRenderer.h"
#include "GUIScreen.h"
#include <string>


//Represents a single element in the GUI.
//TODO: GUI elements can be treated like particles. Use the HGP system to generate materials for GUI elements -- one material for the text rendering, and one for the background image rendering.
//TODO: Finish this class.
class GUIElement
{
public:

    static const std::string RenderedTextSamplerUniformName, BackgroundImageSamplerUniformName;
    //TODO: Coroutine uniform for "Normal", "Selected", and "Clicked" states.


    GUIScreen & Screen;

    TextRenderer & TextRender;
    ManbilTexture BackgroundImage;

    Material * RenderMat;
    UniformDictionary RenderParams;


    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }


    GUIElement(TextRenderer & textRenderer, GUIScreen & screen, TextRenderer::FontSlot _textRenderSlot = TextRenderer::FontSlot())
        : TextRender(textRenderer), Screen(screen), textRenderSlot(_textRenderSlot)
    {

    }


    //Gets the text currently being rendered. If it doesn't exist, this instance's error message will be set and an empty string will be returned.
    std::string GetCurrentText(void) const
    {
        const char * strng = TextRender.GetString(textRenderSlot);

        if (strng == 0)
        {
            errorMsg = "No text found in this element's text render slot.";
            strng = "";
        }

        return strng;
    }
    //Changes this instance's text. Returns whether or not it succeeded.
    //If it failed, the error message can be accessed through "TextRender.GetError()".
    bool ChangeText(std::string newText)
    {
        Vector2i screenFinalRenderSize = Screen.GetFinalRenderSize();
        return TextRender.RenderString(textRenderSlot, newText, (unsigned int)screenFinalRenderSize.x, (unsigned int)screenFinalRenderSize.y);
    }


private:

    mutable std::string errorMsg;

    TextRenderer::FontSlot textRenderSlot;
};