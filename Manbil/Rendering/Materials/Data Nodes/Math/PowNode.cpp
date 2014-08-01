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

const DataLine & PowNode::GetBaseInput(void) const { return GetInputs()[0]; }
const DataLine & PowNode::GetExponentInput(void) const { return GetInputs()[1]; }

PowNode::PowNode(const DataLine & base, const DataLine & exponent, std::string name = "")
    : DataNode(MakeVector(base, exponent),
               [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new PowNode(ins[0], ins[1], _name)); },
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