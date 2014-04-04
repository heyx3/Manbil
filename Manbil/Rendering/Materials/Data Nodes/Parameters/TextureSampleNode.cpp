#include "TextureSampleNode.h"

#include "../DataNodeIncludes.h"



DataNodePtr TextureSampleNode::CreateComplexTexture(std::string samplerName, DataLine scale, DataLine pan, DataLine offset, DataLine uvs)
{
    DataLine scaled = (!scale.IsConstant(Vector2f(1.0f, 1.0f)) ?
                          DataLine(DataNodePtr(new MultiplyNode(uvs, scale)), 0) :
                          uvs);
    DataLine offsetted = (!offset.IsConstant(Vector2f(0.0f, 0.0f)) ?
                             DataLine(DataNodePtr(new AddNode(scaled, offset)), 0) :
                             scaled);
    DataLine panned = (!pan.IsConstant(Vector2f(0.0f, 0.0f)) ?
                          DataLine(DataNodePtr(new AddNode(offsetted,
                                                           DataLine(DataNodePtr(new MultiplyNode(pan,
                                                                                                 DataLine(DataNodePtr(new TimeNode()), 0))), 0))), 0) :
                          offsetted);

    return DataNodePtr(new TextureSampleNode(samplerName, panned));
}

std::string TextureSampleNode::GetOutputName(unsigned int index) const
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
        default: assert(false);
    }
}

unsigned int TextureSampleNode::GetOutputIndex(ChannelsOut channel)
{
    switch (channel)
    {
        case ChannelsOut::CO_Red: return 0;
        case ChannelsOut::CO_Green: return 1;
        case ChannelsOut::CO_Blue: return 2;
        case ChannelsOut::CO_Alpha: return 3;
        case ChannelsOut::CO_AllColorChannels: return 4;
        case ChannelsOut::CO_AllChannels: return 5;

        default: assert(false);
    }
}


TextureSampleNode::TextureSampleNode(std::string _samplerName, DataLine UVs)
    : DataNode(MakeVector(UVs), makeVector())
{
    assert(UVs.GetDataLineSize() == 2);

    samplerName = _samplerName;
    if (samplerName.empty())
        samplerName = GetName() + std::to_string(GetUniqueID()) + "_" + "sample";
}


void TextureSampleNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture2D(" + GetSamplerUniformName() + ", " + GetUVInput().GetValue() + ");\n";
}