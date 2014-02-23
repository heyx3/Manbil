#include "TextureSampleNode.h"

void TextureSampleNode::WriteMyOutputs(std::string & outCode) const
{
    std::string uv = GetUVInput().GetValue(),
                uvScale = GetUVScaleInput().GetValue(),
                uvPan = GetUVPanInput().GetValue(),
                uvOffset = GetUVOffsetInput().GetValue(),
                time = GetTimeInput().GetValue();
    std::string uvFinal = std::string() + "(" + uvScale + " * (" + uv + " + (" + uvOffset + " + (" + time + " * " + uvPan + "))))";


    outCode += "\tvec4 " + GetSampleOutputName() + " = texture2D(" + GetSamplerUniformName() + ", " + uvFinal + ");\n";
}