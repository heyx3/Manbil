#include "MaxMinNodes.h"


unsigned int MaxNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return BasicMath::Max(GetInputs()[0].GetSize(), GetInputs()[1].GetSize());
}
std::string MaxNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_maxed";
}

MaxNode::MaxNode(const DataLine & in1, const DataLine & in2, std::string name = "")
    : DataNode(MakeVector(in1, in2),
               []() { return DataNodePtr(new MaxNode(DataLine(0.0f), DataLine(0.0f))); },
               name)
{
    Assert(in1.GetSize() == 1 || in2.GetSize() == 1 ||
           in1.GetSize() == in2.GetSize(),
           "Both inputs need to be either size 1 or the same size!");
}

void MaxNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
    outCode += "\t" + vecType + " " + GetOutputName(0) + " = max(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}


unsigned int MinNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return BasicMath::Max(GetInputs()[0].GetSize(), GetInputs()[1].GetSize());
}
std::string MinNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_maxed";
}

MinNode::MinNode(const DataLine & in1, const DataLine & in2, std::string name = "")
    : DataNode(MakeVector(in1, in2),
               []() { return DataNodePtr(new MinNode(DataLine(0.0f), DataLine(0.0f))); },
               name)
{
    Assert(in1.GetSize() == 1 || in2.GetSize() == 1 ||
           in1.GetSize() == in2.GetSize(),
           "Both inputs need to be either size 1 or the same size!");
}

void MinNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
    outCode += "\t" + vecType + " " + GetOutputName(0) + " = min(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}