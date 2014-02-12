#include "TextureSampleNode.h"

void TextureSampleNode::WriteMyOutputs(std::string & outCode) const
{
    std::string uv = GetUVInput().GetValue(),
                uvScale = GetUVScaleInput().GetValue(),
                uvPan = GetUVPanInput().GetValue(),
                uvOffset = GetUVOffsetInput().GetValue(),
                time = GetTimeInput().GetValue();
    std::string uvFinal = std::string() + "(" + uv + " + (" + uvScale + " * (" + uvOffset + " + (" + time + " * " + uvPan + "))))";
    std::string outType = Vector(GetSize(channel)).GetGLSLType();


    outCode += "\t" + outType + " " + GetOutputName(0, ) + " = texture2D(" + GetSamplerUniformName() + ", " + uvFinal + ")";


    switch (channel)
    {
        case ChannelsOut::CO_Red:
            outCode += ".x";
            break;
        case ChannelsOut::CO_Green:
            outCode += ".y";
            break;
        case ChannelsOut::CO_Blue:
            outCode += ".z";
            break;
        case ChannelsOut::CO_Alpha:
            outCode += ".w";
            break;
        case ChannelsOut::CO_AllColorChannels:
            outCode += ".xyz";
            break;
        case ChannelsOut::CO_AllChannels:
            //No need to use swizzles.
            break;

        default: assert(false);
    }


    outCode += ";\n";
}

std::string TextureSampleNode::GetOutputName(unsigned int index) const
{
    assert(index == 0);

    std::string ret = GetName() + std::to_string(GetUniqueID()) + "_";

    switch (channel)
    {
    case ChannelsOut::CO_Red: ret += "red"; break;
    case ChannelsOut::CO_Green: ret += "green"; break;
    case ChannelsOut::CO_Blue: ret += "blue"; break;
    case ChannelsOut::CO_Alpha: ret += "alpha"; break;
    case ChannelsOut::CO_AllColorChannels: ret += "color"; break;
    case ChannelsOut::CO_AllChannels: ret += "texVal"; break;
    }

    return ret;
}