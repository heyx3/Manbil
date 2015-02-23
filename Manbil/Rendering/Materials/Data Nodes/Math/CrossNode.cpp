#include "CrossNode.h"


ADD_NODE_REFLECTION_DATA_CPP(CrossNode, Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f))


#pragma warning(disable: 4100)
unsigned int CrossNode::GetOutputSize(unsigned int index) const
{
    return 3;
}
std::string CrossNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_crossed";
}
#pragma warning(default: 4100)

CrossNode::CrossNode(const DataLine & firstL, const DataLine & secondL, std::string name)
    : DataNode(MakeVector(firstL, secondL), name)
{
}

void CrossNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec3 " + GetOutputName(0) + " = cross(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}

void CrossNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 3, "First input isn't a vec3!");
    Assert(GetInputs()[1].GetSize() == 3, "Second input isn't a vec3!");
}