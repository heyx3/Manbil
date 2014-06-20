#include "GUIElement.h"

#include "../Materials/Data Nodes/DataNodeIncludes.h"



DataLine GUIElement::TextImageSampler(DataLine uvs, DataLine textScale, DataLine imgColor, DataLine textColor)
{
    DataLine sampleImage(DataNodePtr(new TextureSampleNode(uvs, BackgroundImageSamplerUniformName)),
                         TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllChannels));

    //Text is sampled using a texture that only has a red component.
    DataLine sampleTextRed(DataNodePtr(new TextureSampleNode(uvs, RenderedTextSamplerUniformName)),
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
    return (DataNodePtr(new AddNode(DataLine(DataNodePtr(new MultiplyNode(finalImgColorMultiply)), 0),
                                    DataLine(DataNodePtr(new MultiplyNode(finalTextColorMultiply)), 0))), 0);
}






/*
namespace GUIElementSubroutineGenerator
{
    std::shared_ptr<SubroutineDefinition> GenerateGUIElementSubroutineUniform(void)
    {

        return std::shared_ptr<SubroutineDefinition>(new SubroutineDefinition(ShaderHandler))
        return UniformSubroutineValue();
    }
}
*/


const std::string GUIElement::RenderedTextSamplerUniformName = "u_GUITextSampler",
                  GUIElement::BackgroundImageSamplerUniformName = "u_GUIBackgroundSampler";
//std::shared_ptr<SubroutineDefinition> GUIElement::elementStateSubroutine = GUIElementSubroutineGenerator::GenerateGUIElementSubroutineUniform();