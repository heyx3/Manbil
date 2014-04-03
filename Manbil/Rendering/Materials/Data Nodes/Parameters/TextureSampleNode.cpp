#include "TextureSampleNode.h"

void TextureSampleNode::WriteMyOutputs(std::string & outCode) const
{
    std::string uv = GetUVInput().GetValue(),
                uvScale = GetUVScaleInput().GetValue(),
                uvPan = GetUVPanInput().GetValue(),
                uvOffset = GetUVOffsetInput().GetValue(),
                time = GetTimeInput().GetValue();

    bool usesScale = !GetUVScaleInput().IsConstant() ||
                     GetUVScaleInput().GetConstantValue() != VectorF(1.0f, 1.0f),
         usesPan = !GetUVPanInput().IsConstant() ||
                    GetUVPanInput().GetConstantValue() != VectorF(0.0f, 0.0f),
         usesOffset = !GetUVOffsetInput().IsConstant() ||
                      GetUVOffsetInput().GetConstantValue() != VectorF(0.0f, 0.0f);


    std::string uvFinal = std::string() +
                          (usesScale ? "(" + uvScale + " * " : "") +
                              "(" + uv +
                                   (usesOffset ? " + (" + uvOffset + " + " : "") +
                                   (usesPan ? "(" + time + " * " + uvPan + ")" : "") +
                              ")" + (usesPan ? ")" : "") + (usesOffset ? ")" : "") + (usesScale ? ")" : "");
    outCode += "\tvec4 " + GetSampleOutputName() + " = texture2D(" + GetSamplerUniformName() + ", " + uvFinal + ");\n";
}