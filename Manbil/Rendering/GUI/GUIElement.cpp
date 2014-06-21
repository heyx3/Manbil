#include "GUIElement.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"



DataLine GUIElement::TextImageSampler(DataLine uvs, DataLine textScale, DataLine imgColor, DataLine textColor)
{
    DataLine sampleImage(DataNodePtr(new TextureSampleNode(uvs, BackgroundImageSamplerUniformName)),
                         TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllChannels));

    //Text is sampled using a texture that only has a red component.
    DataLine sampleTextRed(DataNodePtr(new TextureSampleNode(DataLine(DataNodePtr(new MultiplyNode(uvs, textScale)), 0),
                                                             RenderedTextSamplerUniformName)),
                           TextureSampleNode::GetOutputIndex(ChannelsOut::CO_Red));
    DataLine sampleText = DataLine(DataNodePtr(new CombineVectorNode(sampleTextRed, sampleTextRed, sampleTextRed, sampleTextRed)), 0);

    //Blend the text on top of the image.
    std::vector<DataLine> finalImgColorMultiply;
    finalImgColorMultiply.insert(finalImgColorMultiply.end(), sampleImage);
    finalImgColorMultiply.insert(finalImgColorMultiply.end(), imgColor);
    finalImgColorMultiply.insert(finalImgColorMultiply.end(), DataLine(DataNodePtr(new OneMinusNode(sampleTextRed)), 0));
    std::vector<DataLine> finalTextColorMultiply;
    finalTextColorMultiply.insert(finalTextColorMultiply.end(), sampleText);
    finalTextColorMultiply.insert(finalTextColorMultiply.end(), textColor);
    finalTextColorMultiply.insert(finalTextColorMultiply.end(), sampleTextRed);
    return DataLine(DataNodePtr(new AddNode(DataLine(DataNodePtr(new MultiplyNode(finalImgColorMultiply)), 0),
                                            DataLine(DataNodePtr(new MultiplyNode(finalTextColorMultiply)), 0))), 0);
}



const std::string GUIElement::RenderedTextSamplerUniformName = "u_GUITextSampler",
                  GUIElement::BackgroundImageSamplerUniformName = "u_GUIBackgroundSampler";
const std::string GUIElement::ButtonTimeUniformName = "u_buttonTime";
const std::string GUIElement::WorldPosSubroutineName = "sub_worldPosCalc",
                  GUIElement::WorldPosSubroutineUniformName = "u_worldPosCalc",
                  GUIElement::WorldPos_Normal_SubroutineFuncName = "u_normal_worldPosCalc",
                  GUIElement::WorldPos_Selected_SubroutineFuncName = "u_selected_worldPosCalc",
                  GUIElement::WorldPos_Clicked_SubroutineFuncName = "u_clicked_worldPosCalc";
const std::string GUIElement::RotationSubroutineName = "sub_rotationCalc",
                  GUIElement::RotationSubroutineUniformName = "u_rotationCalc",
                  GUIElement::Rotation_Normal_SubroutineFuncName = "u_normal_rotationCalc",
                  GUIElement::Rotation_Selected_SubroutineFuncName = "u_selected_rotationCalc",
                  GUIElement::Rotation_Clicked_SubroutineFuncName = "u_clicked_rotationCalc";
const std::string GUIElement::ColorSubroutineName = "sub_colorCalc",
                  GUIElement::ColorSubroutineUniformName = "u_colorCalc",
                  GUIElement::Color_Normal_SubroutineFuncName = "u_normal_colorCalc",
                  GUIElement::Color_Selected_SubroutineFuncName = "u_selected_colorCalc",
                  GUIElement::Color_Clicked_SubroutineFuncName = "u_clicked_colorCalc";
const std::string GUIElement::SizeSubroutineName = "sub_colorCalc",
                  GUIElement::SizeSubroutineUniformName = "u_colorCalc",
                  GUIElement::Size_Normal_SubroutineFuncName = "u_normal_colorCalc",
                  GUIElement::Size_Selected_SubroutineFuncName = "u_selected_colorCalc",
                  GUIElement::Size_Clicked_SubroutineFuncName = "u_clicked_colorCalc";

unsigned int GUIElement::GetFunctionIDIndex(std::string uniformName, std::string functionName) const
{
    const unsigned int possibleSubroutineValues = 3;


    //Find the uniform.
    auto loc = RenderParams.SubroutineUniforms.find(uniformName);
    if (loc == RenderParams.SubroutineUniforms.end())
    {
        return possibleSubroutineValues;
    }

    //Find the function.
    const UniformSubroutineValue & subroutine = loc->second;
    for (int i = 0; i < subroutine.PossibleValues.size(); ++i)
    {
        if (subroutine.PossibleValues[i] == functionName)
        {
            return i;
        }
    }

    return possibleSubroutineValues;
}

bool GUIElement::SetSubroutineFunctionID(std::string subroutineUniform, unsigned int index)
{
    auto uniformLoc = RenderParams.SubroutineUniforms.find(subroutineUniform);
    if (uniformLoc == RenderParams.SubroutineUniforms.end())
        return false;

    RenderParams.SubroutineUniforms[subroutineUniform].ValueIndex = index;

    return true;
}