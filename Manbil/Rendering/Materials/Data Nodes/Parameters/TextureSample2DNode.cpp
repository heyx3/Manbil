#include "TextureSample2DNode.h"

#include "../DataNodeIncludes.h"


std::string TextureSample2DNode::GetOutputName(unsigned int index) const
{
    std::string base = GetSampleOutputName();

    switch (index)
    {
        case 0: return base + ".x";
        case 1: return base + ".y";
        case 2: return base + ".z";
        case 3: return base + ".w";
        case 4: return base + ".xyz";
        case 5: return base;
        default:
            Assert(false, std::string() + "Invalid output index: " + ToString(index));
            return std::string() + "ERROR_BAD_OUTPUT_" + ToString(index);
    }
}

unsigned int TextureSample2DNode::GetOutputIndex(ChannelsOut channel)
{
    switch (channel)
    {
        case ChannelsOut::CO_Red: return 0;
        case ChannelsOut::CO_Green: return 1;
        case ChannelsOut::CO_Blue: return 2;
        case ChannelsOut::CO_Alpha: return 3;
        case ChannelsOut::CO_AllColorChannels: return 4;
        case ChannelsOut::CO_AllChannels: return 5;

        default: assert(false); return 999;
    }
}


void TextureSample2DNode::GetMyParameterDeclarations(UniformDictionary & uniforms) const
{
    uniforms.Texture2DUniforms[GetSamplerUniformName()] = UniformSampler2DValue(GetSamplerUniformName());
}


TextureSample2DNode::TextureSample2DNode(const DataLine & uvs, std::string _samplerName)
    : DataNode(MakeVector(uvs), makeVector())
{
    Assert(uvs.GetDataLineSize() == 2,
           std::string() + "UV input isn't size 2; it's size " + ToString(uvs.GetDataLineSize()));

    samplerName = _samplerName;
    if (samplerName.empty())
        samplerName = GetName() + ToString(GetUniqueID()) + "_" + "sample";
}


void TextureSample2DNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture2D(" + GetSamplerUniformName() + ", " + GetUVInput().GetValue() + ");\n";
}

std::vector<unsigned int> TextureSample2DNode::makeVector(void)
{
    std::vector<unsigned int> ints;
    ints.insert(ints.end(), 1);
    ints.insert(ints.end(), 1);
    ints.insert(ints.end(), 1);
    ints.insert(ints.end(), 1);
    ints.insert(ints.end(), 3);
    ints.insert(ints.end(), 4);
    return ints;
}

unsigned int TextureSample2DNode::GetSize(ChannelsOut channel)
{
    switch (channel)
    {
        case ChannelsOut::CO_Red:
        case ChannelsOut::CO_Green:
        case ChannelsOut::CO_Blue:
        case ChannelsOut::CO_Alpha:
            return 1;
        case ChannelsOut::CO_AllColorChannels:
            return 3;
        case ChannelsOut::CO_AllChannels:
            return 4;

        default: assert(false); return 0;
    }
}