#include "InterpolateNode.h"

InterpolateNode::InterpolateNode(DataLine min, DataLine max, DataLine interp, InterpolationType type)
    : DataNode(MakeVector(min, max, interp), MakeVector(BasicMath::Max(max.GetDataLineSize(), interp.GetDataLineSize())))
{
    assert(type != IT_Power);
    intType = type;

    unsigned int minS = min.GetDataLineSize(),
                 maxS = max.GetDataLineSize(),
                 intS = interp.GetDataLineSize();

    assert(minS == maxS && (intS == minS || intS == 1));
}

InterpolateNode::InterpolateNode(DataLine min, DataLine max, DataLine interp, DataLine power)
    : DataNode(MakeVector(min, max, interp, power), MakeVector(BasicMath::Max(max.GetDataLineSize(), interp.GetDataLineSize())))
{
    intType = InterpolationType::IT_Power;

    unsigned int minS = min.GetDataLineSize(),
                 maxS = max.GetDataLineSize(),
                 intS = interp.GetDataLineSize();

    assert(minS == maxS && (intS == minS || intS == 1));
}


void InterpolateNode::GetMyFunctionDeclarations(std::vector<std::string> & outFuncs) const
{
    if (intType != InterpolationType::IT_VerySmooth) return;

    std::string vType = VectorF(GetMinInput().GetDataLineSize()).GetGLSLType();
    std::string vType2 = VectorF(GetInterpInput().GetDataLineSize()).GetGLSLType();
    std::string funcName = GetName() + "_verySmoothStep";

    std::string str = vType + funcName + "(" + vType + " minV, " + vType + " maxV, " + vType2 + " interpolantV)\n\
                      {\n\
                         \tfloat interpValue = interpolant * interpolant * interpolant *\n\
                            \t\t(10.0 + (interpolant * (-15.0 + (6.0 * interpolant))));\n\
                         \treturn minV + (interpValue * (maxV - minV));\n\
                      }\n";

    outFuncs.insert(outFuncs.begin(), str);
}

void InterpolateNode::WriteMyOutputs(std::string & outCode) const
{
    std::string minMaxType = VectorF(GetMinInput().GetDataLineSize()).GetGLSLType(),
                interpType = VectorF(GetInterpInput().GetDataLineSize()).GetGLSLType(),
                returnType = VectorF(BasicMath::Max(GetMinInput().GetDataLineSize(), GetInterpInput().GetDataLineSize())).GetGLSLType();

    switch (intType)
    {
    case IT_Linear:
        outCode += "\t" + returnType + " " + GetOutputName(0) + " = mix(" + GetMinInput().GetValue() + ", " +
                                                                            GetMaxInput().GetValue() + ", " +
                                                                            GetInterpInput().GetValue() + ");\n";
        break;

    case IT_Smooth:
        outCode += "\t" + returnType + " " + GetOutputName(0) + " = smoothstep(" + GetMinInput().GetValue() + ", " +
                                                                                   GetMaxInput().GetValue() + ", " +
                                                                                   GetInterpInput().GetValue() + ");\n";
        break;

    case IT_VerySmooth:
        outCode += "\t" + returnType + " " + GetOutputName(0) + " = " + GetName() + "_verySmoothStep(" + GetMinInput().GetValue() + ", " +
                                                                                                         GetMaxInput().GetValue() + ", " +
                                                                                                         GetInterpInput().GetValue() + ");\n";
        break;

    case IT_Power:
        outCode += "\t" + returnType + " " + GetOutputName(0) + " = mix(" + GetMinInput().GetValue() + ", " +
                                                                            GetMaxInput().GetValue() + ", " +
                                                                            "pow(" + GetInterpInput().GetValue() + ", " +
                                                                                     GetPowerInput().GetValue() + "));\n";
        break;

    default: assert(false);
    }
}