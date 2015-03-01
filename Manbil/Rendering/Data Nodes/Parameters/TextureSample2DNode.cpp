#include "TextureSample2DNode.h"


ADD_NODE_REFLECTION_DATA_CPP(TextureSample2DNode, Vector2f())


std::string TextureSample2DNode::GetOutputName(unsigned int index) const
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
unsigned int TextureSample2DNode::GetOutputSize(unsigned int index) const
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


TextureSample2DNode::TextureSample2DNode(const DataLine & uvs, std::string samplerName, std::string name)
    : DataNode(MakeVector(uvs), name),
      SamplerName(samplerName)
{
    if (SamplerName.empty()) SamplerName = "u_" + GetName() + "_2DTex";
}


void TextureSample2DNode::GetMyParameterDeclarations(UniformDictionary & uniforms) const
{
    uniforms.Texture2Ds[SamplerName] = UniformValueSampler2D(SamplerName);
}
void TextureSample2DNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture2D(" + SamplerName + ", " + GetInputs()[0].GetValue() + ");\n";
}

void TextureSample2DNode::AssertMyInputsValid(void) const
{
    //Make sure the param name is valid.
    Assert(MaterialConstants::IsValidGLSLName(SamplerName),
           "Parameter name '" + SamplerName + "' isn't a valid GLSL variable name!");

    Assert(GetInputs()[0].GetSize() == 2, "UV input isn't size 2; it's size " + ToString(GetInputs()[0].GetSize()));
}


#pragma warning(disable: 4100)
std::string TextureSample2DNode::GetInputDescription(unsigned int index) const
{
    return "Tex Coord 2D";
}
#pragma warning(default: 4100)


void TextureSample2DNode::WriteExtraData(DataWriter* writer) const
{
    writer->WriteString(SamplerName, "Sampler name uniform");
}
void TextureSample2DNode::ReadExtraData(DataReader* reader)
{
    reader->ReadString(SamplerName);
}
