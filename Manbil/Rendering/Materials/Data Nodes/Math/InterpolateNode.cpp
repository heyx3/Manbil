#include "InterpolateNode.h"


MAKE_NODE_READABLE_CPP(InterpolateNode, 0.0f, 1.0f, 0.5f, InterpolateNode::IT_Linear)

unsigned int InterpolateNode::GetOutputSize(unsigned int index) const
{
    return BasicMath::Max(GetMaxInput().GetSize(), GetInterpInput().GetSize());
}
std::string InterpolateNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_interpolated";
}


InterpolateNode::InterpolateNode(DataLine min, DataLine max, DataLine interp, InterpolationType type, std::string name)
    : DataNode(MakeVector(min, max, interp), name),
      intType(type)
{

}


void InterpolateNode::GetMyFunctionDeclarations(std::vector<std::string> & outFuncs) const
{
    if (intType != InterpolationType::IT_VerySmooth) return;

    std::string vType = VectorF(GetMinInput().GetSize()).GetGLSLType();
    std::string vType2 = VectorF(GetInterpInput().GetSize()).GetGLSLType();
    std::string funcName = GetName() + "_verySmoothStep";

    //TODO: See if part of this function can be replaced with a single mix() or smoothstep().
    std::string str = vType + " " + funcName + "(" + vType + " minV, " + vType + " maxV, " + vType2 + " interpolantV)\n\
                      {\n\
                         \tfloat interpValue = interpolantV * interpolantV * interpolantV *\n\
                            \t\t(10.0 + (interpolantV * (-15.0 + (6.0 * interpolantV))));\n\
                         \treturn minV + (interpValue * (maxV - minV));\n\
                      }\n";

    outFuncs.insert(outFuncs.begin(), str);
}
void InterpolateNode::WriteMyOutputs(std::string & outCode) const
{
    std::string minMaxType = VectorF(GetMinInput().GetSize()).GetGLSLType(),
                interpType = VectorF(GetInterpInput().GetSize()).GetGLSLType(),
                returnType = VectorF(BasicMath::Max(GetMinInput().GetSize(), GetInterpInput().GetSize())).GetGLSLType();

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

    default:
        Assert(false, "Unknown interpolant type: " + std::to_string(intType));
    }
}


std::string InterpolateNode::GetInputDescription(unsigned int index) const
{
    return (index == 0 ? "Min" : (index == 1 ? "Max" : "To Interpolate"));
}


bool InterpolateNode::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    unsigned int smoothLevel;
    switch (intType)
    {
        case IT_Linear:
            smoothLevel = 0;
            break;
        case IT_Smooth:
            smoothLevel = 1;
            break;
        case IT_VerySmooth:
            smoothLevel = 2;
            break;

        default:
            outError = "Unknown interpolation type '" + ToString(intType);
            return false;
    }

    if (!writer->WriteUInt(smoothLevel, "Smooth Level (0-2)", outError))
    {
        outError = "Error writing out smooth level of " + ToString(smoothLevel) + ": " + outError;
        return false;
    }

    return true;
}
bool InterpolateNode::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<unsigned int> trySmoothLevel = reader->ReadUInt(outError);
    if (!trySmoothLevel.GetValue())
    {
        outError = "Error reading smooth level: " + outError;
        return false;
    }

    switch (trySmoothLevel.GetValue())
    {
        case 0:
            intType = IT_Linear;
            break;
        case 1:
            intType = IT_Smooth;
            break;
        case 2:
            intType = IT_VerySmooth;
            break;

        default:
            outError = "Smooth level must be between 0 and 2 inclusive; it was " + ToString(trySmoothLevel.GetValue());
        return false;
    }

    return true;
}

void InterpolateNode::AssertMyInputsValid(void) const
{
    unsigned int minS = GetMinInput().GetSize(),
                 maxS = GetMaxInput().GetSize(),
                 intS = GetInterpInput().GetSize();

    Assert(minS == maxS, "'min' and 'max' are different sizes!");
    Assert(intS == minS || intS == 1, "interpolant must be size 1 or the size of the min/max inputs!");
}