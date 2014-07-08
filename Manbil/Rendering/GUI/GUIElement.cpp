#include "GUIElement.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"



DataLine GUIElement2::TextImageSampler(DataLine uvs, DataLine textScale, DataLine imgColor, DataLine textColor)
{
    DataLine sampleImage(DataNodePtr(new TextureSample2DNode(uvs, BackgroundImageSamplerUniformName)),
                         TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllChannels));

    //Text is sampled using a texture that only has a red component.
    DataLine sampleTextRed(DataNodePtr(new TextureSample2DNode(DataLine(DataNodePtr(new MultiplyNode(uvs, textScale)), 0),
                                                             RenderedTextSamplerUniformName)),
                           TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_Red));
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



const std::string GUIElement2::RenderedTextSamplerUniformName = "u_GUITextSampler",
                  GUIElement2::BackgroundImageSamplerUniformName = "u_GUIBackgroundSampler";
const std::string GUIElement2::ButtonTimeUniformName = "u_buttonTime";
const std::string GUIElement2::WorldPosSubroutineName = "sub_worldPosCalc",
                  GUIElement2::WorldPosSubroutineUniformName = "u_worldPosCalc",
                  GUIElement2::WorldPos_Normal_SubroutineFuncName = "u_normal_worldPosCalc",
                  GUIElement2::WorldPos_Selected_SubroutineFuncName = "u_selected_worldPosCalc",
                  GUIElement2::WorldPos_Clicked_SubroutineFuncName = "u_clicked_worldPosCalc";
const std::string GUIElement2::RotationSubroutineName = "sub_rotationCalc",
                  GUIElement2::RotationSubroutineUniformName = "u_rotationCalc",
                  GUIElement2::Rotation_Normal_SubroutineFuncName = "u_normal_rotationCalc",
                  GUIElement2::Rotation_Selected_SubroutineFuncName = "u_selected_rotationCalc",
                  GUIElement2::Rotation_Clicked_SubroutineFuncName = "u_clicked_rotationCalc";
const std::string GUIElement2::ColorSubroutineName = "sub_colorCalc",
                  GUIElement2::ColorSubroutineUniformName = "u_colorCalc",
                  GUIElement2::Color_Normal_SubroutineFuncName = "u_normal_colorCalc",
                  GUIElement2::Color_Selected_SubroutineFuncName = "u_selected_colorCalc",
                  GUIElement2::Color_Clicked_SubroutineFuncName = "u_clicked_colorCalc";
const std::string GUIElement2::SizeSubroutineName = "sub_colorCalc",
                  GUIElement2::SizeSubroutineUniformName = "u_colorCalc",
                  GUIElement2::Size_Normal_SubroutineFuncName = "u_normal_colorCalc",
                  GUIElement2::Size_Selected_SubroutineFuncName = "u_selected_colorCalc",
                  GUIElement2::Size_Clicked_SubroutineFuncName = "u_clicked_colorCalc";

unsigned int GUIElement2::GetFunctionIDIndex(std::string uniformName, std::string functionName) const
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

bool GUIElement2::SetSubroutineFunctionID(std::string subroutineUniform, unsigned int index)
{
    auto uniformLoc = RenderParams.SubroutineUniforms.find(subroutineUniform);
    if (uniformLoc == RenderParams.SubroutineUniforms.end())
        return false;

    RenderParams.SubroutineUniforms[subroutineUniform].ValueIndex = index;

    return true;
}