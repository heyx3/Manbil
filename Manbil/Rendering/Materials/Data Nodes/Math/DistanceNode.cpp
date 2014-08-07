#include "DistanceNode.h"


MAKE_NODE_READABLE_CPP(DistanceNode, Vector3f(), Vector3f())

unsigned int DistanceNode::GetOutputSize(unsigned int index) const
{
    return 1;
}
std::string DistanceNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_distance";
}

DistanceNode::DistanceNode(const DataLine & pos1, const DataLine & pos2, std::string name)
    : DataNode(MakeVector(pos1, pos2), name)
{
}

void DistanceNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
    outCode += "\t" + vecType + " " + GetOutputName(0) + " = distance(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}

void DistanceNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == GetInputs()[1].GetSize(), "Both inputs must be the same size!");
}