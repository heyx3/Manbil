#include "MaxMinNodes.h"


MAKE_NODE_READABLE_CPP(MaxNode, 1.0f, 1.0f)
MAKE_NODE_READABLE_CPP(MinNode, 1.0f, 1.0f)


unsigned int MaxNode::GetOutputSize(unsigned int index) const
{
    return BasicMath::Max(GetInputs()[0].GetSize(), GetInputs()[1].GetSize());
}
std::string MaxNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_maxed";
}

MaxNode::MaxNode(const DataLine & in1, const DataLine & in2, std::string name)
    : DataNode(MakeVector(in1, in2), name)
{
}

void MaxNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
    outCode += "\t" + vecType + " " + GetOutputName(0) + " = max(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}

void MaxNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 1 || GetInputs()[1].GetSize() == 1 ||
             GetInputs()[0].GetSize() == GetInputs()[1].GetSize(),
           "Both inputs need to be either size 1 or the same size!");
}


unsigned int MinNode::GetOutputSize(unsigned int index) const
{
    return BasicMath::Max(GetInputs()[0].GetSize(), GetInputs()[1].GetSize());
}
std::string MinNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_maxed";
}

MinNode::MinNode(const DataLine & in1, const DataLine & in2, std::string name)
    : DataNode(MakeVector(in1, in2), name)
{
}

void MinNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
    outCode += "\t" + vecType + " " + GetOutputName(0) + " = min(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}

void MinNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 1 || GetInputs()[1].GetSize() == 1 ||
             GetInputs()[0].GetSize() == GetInputs()[1].GetSize(),
           "Both inputs need to be either size 1 or the same size!");
}