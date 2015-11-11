#include "InterpolateNode.h"


ADD_NODE_REFLECTION_DATA_CPP(InterpolateNode, 0.0f, 1.0f, 0.5f, InterpolateNode::IT_Linear)

#pragma warning(disable: 4100)
unsigned int InterpolateNode::GetOutputSize(unsigned int index) const
{
    return Mathf::Max(GetMaxInput().GetSize(), GetInterpInput().GetSize());
}
std::string InterpolateNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_interpolated";
}
#pragma warning(default: 4100)


InterpolateNode::InterpolateNode(DataLine min, DataLine max, DataLine interp, InterpolationType type, std::string name)
    : DataNode(MakeVector(min, max, interp), name),
      intType(type)
{

}


void InterpolateNode::GetMyFunctionDeclarations(std::vector<std::string> & outFuncs) const
{
    if (intType != InterpolationType::IT_VerySmooth) return;

    std::string vType = VectorF(GetMinInput().GetSize(), 0).GetGLSLType();
    std::string vType2 = VectorF(GetInterpInput().GetSize(), 0).GetGLSLType();
    std::string funcName = GetName() + "_verySmoothStep";

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
    std::string minMaxType = VectorF(GetMinInput().GetSize(), 0).GetGLSLType(),
                interpType = VectorF(GetInterpInput().GetSize(), 0).GetGLSLType(),
                returnType = VectorF(Mathf::Max(GetMinInput().GetSize(),
                                                GetInterpInput().GetSize()), 0).GetGLSLType();
    std::string interpValue = GetInterpInput().GetValue();

    switch (intType)
    {
        case IT_Linear:
            outCode += "\t" + returnType + " " + GetOutputName(0) + " = mix(" + GetMinInput().GetValue() + ", " +
                                                                                GetMaxInput().GetValue() + ", " +
                                                                                interpValue + ");\n";
            break;

        case IT_Smooth:
            //A problem with smoothstep is that you can't interpolate vectors with a float --
            //   the interpolant must be the same size.
            if (GetMinInput().GetSize() != GetInterpInput().GetSize())
            {
                assert(GetInterpInput().GetSize() == 1);

                std::string oldVal = interpValue;
                interpValue = minMaxType + "(";
                for (unsigned int i = 0; i < GetMinInput().GetSize(); ++i)
                    interpValue += (i > 0 ? (", " + oldVal) : oldVal);
                interpValue += ')';
            }
            outCode += "\t" + returnType + " " + GetOutputName(0) + " = smoothstep(" + GetMinInput().GetValue() + ", " +
                                                                                       GetMaxInput().GetValue() + ", " +
                                                                                       interpValue + ");\n";
            break;

        case IT_VerySmooth:
            outCode += "\t" + returnType + " " + GetOutputName(0) + " = " +
                            GetName() + "_verySmoothStep(" + GetMinInput().GetValue() + ", " +
                            GetMaxInput().GetValue() + ", " +
                            interpValue + ");\n";
            break;

        default:
            Assert(false, "Unknown interpolant type: " + std::to_string(intType));
    }
}


std::string InterpolateNode::GetInputDescription(unsigned int index) const
{
    return (index == 0 ? "Min" : (index == 1 ? "Max" : "To Interpolate"));
}


void InterpolateNode::WriteExtraData(DataWriter* writer) const
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
            writer->ErrorMessage = "Unknown interpolation type '" + ToString((unsigned int)intType);
            throw DataWriter::EXCEPTION_FAILURE;
    }

    writer->WriteUInt(smoothLevel, "Smooth level (0-2)");
}
void InterpolateNode::ReadExtraData(DataReader* reader)
{
    unsigned int smoothLevel;
    reader->ReadUInt(smoothLevel);

    switch (smoothLevel)
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
            reader->ErrorMessage = "Smooth level must be between 0 and 2 inclusive; it was " +
                                        ToString((unsigned int)smoothLevel);
            throw DataReader::EXCEPTION_FAILURE;
    }
}

void InterpolateNode::AssertMyInputsValid(void) const
{
    unsigned int minS = GetMinInput().GetSize(),
                 maxS = GetMaxInput().GetSize(),
                 intS = GetInterpInput().GetSize();

    Assert(minS == maxS, "'min' and 'max' are different sizes!");
    Assert(intS == minS || intS == 1, "interpolant must be size 1 or the size of the min/max inputs!");
}