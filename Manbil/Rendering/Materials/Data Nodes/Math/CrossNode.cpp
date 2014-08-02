#include "CrossNode.h"

unsigned int CrossNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return 3;
}
std::string CrossNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_crossed";
}

CrossNode::CrossNode(const DataLine & firstL, const DataLine & secondL, std::string name)
    : DataNode(MakeVector(firstL, secondL),
               []() { return DataNodePtr(new CrossNode(DataLine(VectorF(1.0f, 0.0f, 0.0f)), DataLine(VectorF(0.0f, 1.0f, 0.0f)))); },
               name)
{
    Assert(GetInputs()[0].GetSize() == 3, "First input isn't a vec3!");
    Assert(GetInputs()[1].GetSize() == 3, "Second input isn't a vec3!");
}

void CrossNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec3 " + GetOutputName(0) + " = cross(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}