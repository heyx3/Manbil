#include "LogNode.h"


ADD_NODE_REFLECTION_DATA_CPP(LogNode, 1.0f, 2.0f)


#pragma warning(disable: 4100)
unsigned int LogNode::GetOutputSize(unsigned int index) const
{
    return GetInputs()[0].GetSize();
}
std::string LogNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_logResult";
}
#pragma warning(default: 4100)

LogNode::LogNode(const DataLine & value, DataLine base, std::string name)
    : DataNode(MakeVector(value, base), name)
{
}

void LogNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetInputs()[0].GetSize(), 0).GetGLSLType();

    if (GetInputs()[1].IsConstant() && GetInputs()[1].GetConstantValue().GetValue()[0] == 2.0f)
    {
        outCode += "\t" + vecType + " " + GetOutputName(0) +
                    " = log2(" + GetInputs()[0].GetValue() + ");\n";
    }
    else
    {
        outCode += "\t" + vecType + " " + GetOutputName(0) +
                        " = log2(" + GetInputs()[0].GetValue() + ") /\n\t\t" +
                            "log2(" + GetInputs()[1].GetValue() + ");\n";
    }
}

std::string LogNode::GetInputDescription(unsigned int index) const
{
    return (index == 0 ? "Value" : "Log Base");
}

void LogNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[1].GetSize() == 1,
           "Base must have a size of 1! It has size " + ToString(GetInputs()[1].GetSize()));
}