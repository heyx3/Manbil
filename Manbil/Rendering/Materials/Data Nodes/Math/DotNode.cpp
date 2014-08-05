#include "DotNode.h"


unsigned int DotNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetInputs()[0].GetSize();
}
std::string DotNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_dotted";
}

DotNode::DotNode(const DataLine & first, const DataLine & second, std::string name)
    : DataNode(MakeVector(first, second),
               []() { return std::shared_ptr<DataNode>(new DotNode(DataLine(1.0f), DataLine(1.0f))); },
               name)
{
    Assert(first.GetSize() == second.GetSize(), "The two inputs must be the same size!");
}

void DotNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetInputs()[0].GetSize()).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) +
                " = dot(" + GetInputs()[0].GetValue() + ", " +
                GetInputs()[1].GetValue() + ");\n";
}