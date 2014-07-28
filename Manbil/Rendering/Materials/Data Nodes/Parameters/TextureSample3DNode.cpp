#include "TextureSample3DNode.h"



unsigned int TextureSample3DNode::GetOutputIndex(ChannelsOut channel)
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


std::string TextureSample3DNode::GetOutputName(unsigned int index) const
{
    switch (index)
    {
        case 0: return GetSampleOutputName() + ".x";
        case 1: return GetSampleOutputName() + ".y";
        case 2: return GetSampleOutputName() + ".z";
        case 3: return GetSampleOutputName() + ".w";
        case 4: return GetSampleOutputName() + ".xyz";
        case 5: return GetSampleOutputName();
        default:
            Assert(false, "Invalid output index: " + ToString(index));
            return std::string() + "ERROR_BAD_OUTPUT_" + ToString(index);
    }
}
unsigned int TextureSample3DNode::GetOutputSize(unsigned int index) const
{
    switch (index)
    {
        case 0: return 1;
        case 1: return 1;
        case 2: return 1;
        case 3: return 1;

        case 4: return 3;
        case 5: return 4;

        default:
            Assert(false, "Invalid output index " + ToString(index));
            return 0;
    }
}


TextureSample3DNode::TextureSample3DNode(const DataLine & uvs, std::string samplerName, std::string name)
    : DataNode(MakeVector(uvs),
               [](std::vector<DataLine> & inputs, std::string name) { return DataNodePtr(new TextureSample3DNode(inputs[0])); },
               name),
      SamplerName(samplerName)
{
    Assert(uvs.GetSize() == 3, "UV input isn't size 3; it's size " + ToString(uvs.GetSize()));

    if (SamplerName.empty()) SamplerName = "u_" + GetName() + "_3DTex";
}


void TextureSample3DNode::GetMyParameterDeclarations(UniformDictionary & uniforms) const
{
    uniforms.Texture3DUniforms[SamplerName] = UniformSampler3DValue(SamplerName);
}
void TextureSample3DNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture(" + SamplerName + ", " + GetInputs()[0].GetValue() + ");\n";
}


bool TextureSample3DNode::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteString(SamplerName, "samplerUniformName", outError))
    {
        outError = "Error writing sampler uniform name '" + SamplerName + "': " + outError;
        return false;
    }

    return true;
}
bool TextureSample3DNode::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> trySName = reader->ReadString(outError);
    if (!trySName.HasValue())
    {
        outError = "Error reading sampler name: " + outError;
        return false;
    }

    return true;
}