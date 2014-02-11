#include "TextureSampleNode.h"

void TextureSampleNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    std::string uv = GetUVInput().GetValue(shaderType),
                uvScale = GetUVScaleInput().GetValue(shaderType),
                uvPan = GetUVPanInput().GetValue(shaderType),
                uvOffset = GetUVOffsetInput().GetValue(shaderType),
                time = GetTimeInput().GetValue(shaderType);
    std::string uvFinal = std::string() + "(" + uv + " + (" + uvScale + " * (" + uvOffset + " + (" + time + " * " + uvPan + "))))";
    std::string outType = Vector(GetSize(channel)).GetGLSLType();


    outCode += "\t" + outType + " " + GetOutputName(0, shaderType) + " = texture2D(" + GetSamplerUniformName() + ", " + uvFinal + ")";


    switch (channel)
    {
        case ChannelsOut::Red:
            outCode += ".x";
            break;
        case ChannelsOut::Green:
            outCode += ".y";
            break;
        case ChannelsOut::Blue:
            outCode += ".z";
            break;
        case ChannelsOut::Alpha:
            outCode += ".w";
            break;
        case ChannelsOut::AllColorChannels:
            outCode += ".xyz";
            break;
        case ChannelsOut::AllChannels:
            //No need to use swizzles.
            break;

        default: assert(false);
    }


    outCode += ";\n";
}

std::string TextureSampleNode::GetOutputName(unsigned int index, Shaders iLostTheGame) const
{
    assert(index == 0);

    std::string ret = GetName() + std::to_string(GetUniqueID()) + "_";

    switch (channel)
    {
    case ChannelsOut::Red: ret += "red"; break;
    case ChannelsOut::Green: ret += "green"; break;
    case ChannelsOut::Blue: ret += "blue"; break;
    case ChannelsOut::Alpha: ret += "alpha"; break;
    case ChannelsOut::AllColorChannels: ret += "color"; break;
    case ChannelsOut::AllChannels: ret += "texVal"; break;
    }

    return ret;
}