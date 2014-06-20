#pragma once

#include "TextRenderer.h"
#include "GUIScreen.h"
#include "../Materials/Data Nodes/DataLine.h"
#include <string>


//Represents a single element in the GUI.
//TODO: GUI elements can be treated like particles. Use the HGP system to generate a material for the GUI stuff.
class GUIElement
{
public:

    //The different states a GUIElement can be in.
    enum States
    {
        //Nothing is happening to the button.
        GES_NORMAL,
        //The button is being hovered over by the mouse, or is currently selected by another input device.
        GES_SELECTED,
        //The button is being activated.
        GES_CLICKED,
    };

    //Gets a DataLine that outputs a vec4 color of the text and background image sampled and blended.
    static DataLine TextImageSampler(DataLine uvs, DataLine textScale,
                                     DataLine imgColor = DataLine(Vector4f(1.0f, 1.0f, 1.0f, 1.0f)),
                                     DataLine textColor = DataLine(Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));

    static const std::string RenderedTextSamplerUniformName, BackgroundImageSamplerUniformName;
    //static std::shared_ptr<SubroutineDefinition> GetElementStateSubroutine(void) { return elementStateSubroutine; }


    GUIScreen & Screen;

    TextRenderer & TextRender;
    ManbilTexture BackgroundImage;

    Material * RenderMat;
    UniformDictionary RenderParams;


    GUIElement(TextRenderer & textRenderer, GUIScreen & screen,
               std::string _normalStateCoroutine, std::string _selectedStateCoroutine, std::string _clickedStateCoroutine,
               TextRenderer::FontSlot _textRenderSlot = TextRenderer::FontSlot())
        : TextRender(textRenderer), Screen(screen), textRenderSlot(_textRenderSlot), state(GES_NORMAL),
          normalStateCoroutine(_normalStateCoroutine), selectedStateCoroutine(_selectedStateCoroutine), clickedStateCoroutine(_clickedStateCoroutine)
    {

    }


    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetError(void) const { return errorMsg; }

    States GetState(void) const { return state; }


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

    //Updates this element, given the delta time and current state.
    void Update(float elapsed, States newState)
    {
        if (newState != state)
        {
            //TODO: Implement subroutine uniform for the different GUIElement states.
            state = newState;
            switch (state)
            {
                case GES_NORMAL:

                    break;
                case GES_SELECTED:

                    break;
                case GES_CLICKED:

                    break;
                
                default: assert(false);
            }
        }
    }


private:

    mutable std::string errorMsg;

    TextRenderer::FontSlot textRenderSlot;
    States state;
    std::string normalStateCoroutine, selectedStateCoroutine, clickedStateCoroutine;


    //static std::shared_ptr<SubroutineDefinition> elementStateSubroutine;
};