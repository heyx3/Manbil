#include "TextureSampleCubemapNode.h"



std::string TextureSampleCubemapNode::GetOutputName(unsigned int index) const
{
    switch (index)
    {
        case 0: return GetSampleOutputName() + ".r";
        case 1: return GetSampleOutputName() + ".g";
        case 2: return GetSampleOutputName() + ".b";
        case 3: return GetSampleOutputName() + ".a";

        case 4: return GetSampleOutputName() + ".rgb";
        case 5: return GetSampleOutputName();

        default:
            Assert(false, std::string() + "Invalid output index " + ToString(index));
            return std::string() + "WTF_BAD_OUTPUT_INDEX_" + ToString(index);
    }
}
unsigned int TextureSampleCubemapNode::GetOutputSize(unsigned int index) const
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

unsigned int TextureSampleCubemapNode::GetOutputIndex(ChannelsOut channel)
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


TextureSampleCubemapNode::TextureSampleCubemapNode(const DataLine & texCoords, std::string _samplerName, std::string name)
    : DataNode(MakeVector(texCoords),
               [](std::vector<DataLine> & inputs, std::string name) { return DataNodePtr(new TextureSampleCubemapNode(inputs[0])); },
               name),
      SamplerName(_samplerName)
{
    Assert(texCoords.GetSize() == 3, "Texture coord input isn't size 3; it's size " + ToString(texCoords.GetSize()));

    if (SamplerName.empty()) SamplerName = "u_" + GetName() + "_cubeTex";
}


void TextureSampleCubemapNode::GetMyParameterDeclarations(UniformDictionary & uniforms) const
{
    uniforms.TextureCubemapUniforms[SamplerName] = UniformSamplerCubemapValue(SamplerName);
}
void TextureSampleCubemapNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture(" + SamplerName + ", " + GetInputs()[0].GetValue() + ");\n";
}


bool TextureSampleCubemapNode::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    if (!writer->WriteString(SamplerName, "samplerUniformName", outError))
    {
        outError = "Error writing sampler uniform name '" + SamplerName + "': " + outError;
        return false;
    }

    return true;
}
bool TextureSampleCubemapNode::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> trySName = reader->ReadString(outError);
    if (!trySName.HasValue())
    {
        outError = "Error reading sampler name: " + outError;
        return false;
    }

    return true;
}