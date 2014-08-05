#include "PowNode.h"


unsigned int PowNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetBaseInput().GetSize();
}
std::string PowNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_powResult";
}

PowNode::PowNode(const DataLine & base, const DataLine & exponent, std::string name)
    : DataNode(MakeVector(base, exponent),
               []() { return Ptr(new PowNode(DataLine(1.0f), DataLine(1.0f))); },
               name)
{
    Assert(exponent.GetSize() == base.GetSize(), "'exponent' input is not the same size as 'base' input!");
}

void PowNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\t" + VectorF(GetBaseInput().GetSize()).GetGLSLType() + " " + GetOutputName(0) +
        " = pow(" + GetBaseInput().GetValue() + ", " +
        GetExponentInput().GetValue() + ");\n";
}

std::string PowNode::GetInputDescription(unsigned int index) const
{
    return (index == 0) ? "Base" : "Exponent";
}