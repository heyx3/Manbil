#include "TextureSample3DNode.h"


ADD_NODE_REFLECTION_DATA_CPP(TextureSample3DNode, Vector3f())


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

        default:
            assert(false);
            return 999;
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
    : DataNode(MakeVector(uvs), name),
      SamplerName(samplerName)
{
    if (SamplerName.empty())
    {
        SamplerName = "u_" + GetName() + "_3DTex";
    }
}


void TextureSample3DNode::GetMyParameterDeclarations(UniformDictionary & uniforms) const
{
    uniforms.Texture3Ds[SamplerName] = UniformValueSampler3D(SamplerName);
}
void TextureSample3DNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture(" + SamplerName + ", " +
                    GetInputs()[0].GetValue() + ");\n";
}

void TextureSample3DNode::AssertMyInputsValid(void) const
{
    //Make sure the param name is valid.
    Assert(MaterialConstants::IsValidGLSLName(SamplerName),
           "Parameter name '" + SamplerName + "' isn't a valid GLSL variable name!");
    Assert(GetInputs()[0].GetSize() == 3,
           "UV input isn't size 3; it's size " + ToString(GetInputs()[0].GetSize()));
}


std::string TextureSample3DNode::GetInputDescription(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return "Tex Coord 3D";
}


void TextureSample3DNode::WriteExtraData(DataWriter* writer) const
{
    writer->WriteString(SamplerName, "Sampler uniform name");
}
void TextureSample3DNode::ReadExtraData(DataReader* reader)
{
    reader->ReadString(SamplerName);
}