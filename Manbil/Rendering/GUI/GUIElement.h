#pragma once

#include "TextRenderer.h"
#include "GUIScreen.h"
#include "../Materials/Data Nodes/DataLine.h"
#include <string>



//Represents a single element in a UI.
//It has a background texture and a collision box relative to the background image.
class GUIElement
{
public:

    //The background texture of the element.
    ManbilTexture BackgroundTex;
    //The width/height of the collision box, not including the width/height of the background texture.
    Vector2i CollDimensionsExtra;
    //The offset of the collision box, relative to the center of the background texture.
    Vector2i CollCenterOffset;
    

    GUIElement(ManbilTexture backgroundTex, Vector2i collDimensionsExtra, Vector2i collCenterOffset)
        : BackgroundTex(backgroundTex), CollDimensionsExtra(collDimensionsExtra), CollCenterOffset(collCenterOffset)
    {

    }


    //Gets the total width/height of the collision box.
    Vector2i GetCollisionDimensions(void) const
    {
        return CollDimensionsExtra + BackgroundTex.GetSize();
    }
};








//Represents a single element in the GUI.
//TODO: GUI elements can be treated like particles. Use the HGP system to generate a material for the GUI stuff.
class GUIElement2
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


    static const std::string RenderedTextSamplerUniformName, BackgroundImageSamplerUniformName;
    static const std::string ButtonTimeUniformName;
    static const std::string WorldPosSubroutineName, WorldPosSubroutineUniformName,
                             RotationSubroutineName, RotationSubroutineUniformName,
                             ColorSubroutineName, ColorSubroutineUniformName,
                             SizeSubroutineName, SizeSubroutineUniformName;
    static const std::string WorldPos_Normal_SubroutineFuncName, WorldPos_Selected_SubroutineFuncName, WorldPos_Clicked_SubroutineFuncName,
                             Rotation_Normal_SubroutineFuncName, Rotation_Selected_SubroutineFuncName, Rotation_Clicked_SubroutineFuncName,
                             Color_Normal_SubroutineFuncName, Color_Selected_SubroutineFuncName, Color_Clicked_SubroutineFuncName,
                             Size_Normal_SubroutineFuncName, Size_Selected_SubroutineFuncName, Size_Clicked_SubroutineFuncName;


    //Gets a DataLine that outputs a vec4 color of the text and background image sampled and blended.
    //TODO: Take in a DataLine for the rendered text width/height, and an argument about text alignment.
    static DataLine TextImageSampler(DataLine uvs, DataLine textScale,
                                     DataLine imgColor = DataLine(Vector4f(1.0f, 1.0f, 1.0f, 1.0f)),
                                     DataLine textColor = DataLine(Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));


    GUIScreen & Screen;

    TextRenderer & TextRender;
    ManbilTexture BackgroundImage;

    Material * RenderMat;
    UniformDictionary RenderParams;


    GUIElement2(TextRenderer & textRenderer, GUIScreen & screen,
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
            state = newState;
            switch (state)
            {
                case GES_NORMAL:

                    unsigned int index = GetFunctionIDIndex(WorldPosSubroutineUniformName, WorldPos_Normal_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(WorldPosSubroutineUniformName, index);

                    index = GetFunctionIDIndex(RotationSubroutineUniformName, Rotation_Normal_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(RotationSubroutineUniformName, index);

                    index = GetFunctionIDIndex(SizeSubroutineUniformName, Size_Normal_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(SizeSubroutineUniformName, index);

                    index = GetFunctionIDIndex(ColorSubroutineUniformName, Color_Normal_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(ColorSubroutineUniformName, index);

                    break;

                case GES_SELECTED:

                    unsigned int index = GetFunctionIDIndex(WorldPosSubroutineUniformName, WorldPos_Selected_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(WorldPosSubroutineUniformName, index);

                    index = GetFunctionIDIndex(RotationSubroutineUniformName, Rotation_Selected_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(RotationSubroutineUniformName, index);

                    index = GetFunctionIDIndex(SizeSubroutineUniformName, Size_Selected_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(SizeSubroutineUniformName, index);

                    index = GetFunctionIDIndex(ColorSubroutineUniformName, Color_Selected_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(ColorSubroutineUniformName, index);

                    break;

                case GES_CLICKED:

                    unsigned int index = GetFunctionIDIndex(WorldPosSubroutineUniformName, WorldPos_Clicked_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(WorldPosSubroutineUniformName, index);

                    index = GetFunctionIDIndex(RotationSubroutineUniformName, Rotation_Clicked_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(RotationSubroutineUniformName, index);

                    index = GetFunctionIDIndex(SizeSubroutineUniformName, Size_Clicked_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(SizeSubroutineUniformName, index);

                    index = GetFunctionIDIndex(ColorSubroutineUniformName, Color_Clicked_SubroutineFuncName);
                    if (index < 3) SetSubroutineFunctionID(ColorSubroutineUniformName, index);

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

    //Returns the number of possible values (i.e. 3) if the given uniform or function isn't found.
    unsigned int GetFunctionIDIndex(std::string uniformName, std::string functionName) const;
    //Returns whether this operation was successful.
    bool SetSubroutineFunctionID(std::string subroutineUniform, unsigned int index);
};