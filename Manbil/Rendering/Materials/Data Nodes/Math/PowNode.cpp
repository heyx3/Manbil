#include "PowNode.h"


MAKE_NODE_READABLE_CPP(PowNode, 1.0f, 1.0f)


unsigned int PowNode::GetOutputSize(unsigned int index) const
{
    return GetBaseInput().GetSize();
}
std::string PowNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_powResult";
}

PowNode::PowNode(const DataLine & base, const DataLine & exponent, std::string name)
    : DataNode(MakeVector(base, exponent), name)
{
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

void PowNode::AssertMyInputsValid(void) const
{
    Assert(GetExponentInput().GetSize() == GetBaseInput().GetSize(),
           "'exponent' input is not the same size as 'base' input!");
}