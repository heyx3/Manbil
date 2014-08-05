#include "LogNode.h"


unsigned int LogNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetInputs()[0].GetSize();
}
std::string LogNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_logResult";
}

LogNode::LogNode(const DataLine & value, DataLine base, std::string name)
    : DataNode(MakeVector(value, base),
               []() { return Ptr(new LogNode(DataLine(1.0f), DataLine(2.0f))); },
               name)
{
    Assert(base.GetSize() == 1, "Base must have a size of 1! It has size " + ToString(base.GetSize()));
}

void LogNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetInputs()[0].GetSize()).GetGLSLType();

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