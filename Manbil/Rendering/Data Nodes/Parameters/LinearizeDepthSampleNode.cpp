#include "LinearizeDepthSampleNode.h"


ADD_NODE_REFLECTION_DATA_CPP(LinearizeDepthSampleNode, 0.5f)


#pragma warning(disable: 4100)
unsigned int LinearizeDepthSampleNode::GetOutputSize(unsigned int i) const
{
    return 1;
}
std::string LinearizeDepthSampleNode::GetOutputName(unsigned int i) const
{
    return GetName() + "_linearized";
}
#pragma warning(default: 4100)

LinearizeDepthSampleNode::LinearizeDepthSampleNode(const DataLine & dIn, std::string name)
    : DataNode(MakeVector(dIn), name)
{

}

#pragma warning(disable: 4100)
void LinearizeDepthSampleNode::SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
{
    flags.EnableFlag(MaterialUsageFlags::DNF_USES_ZNEAR);
    flags.EnableFlag(MaterialUsageFlags::DNF_USES_ZFAR);
}
#pragma warning(default: 4100)

void LinearizeDepthSampleNode::WriteMyOutputs(std::string & outStr) const
{
    std::string zn = MaterialConstants::CameraZNearName,
                zf = MaterialConstants::CameraZFarName,
                d = GetInputs()[0].GetValue();
    outStr += "\tfloat " + GetOutputName(0) + " = (2.0f * " + zn + ") / " +
                                                 "(" + zf + " + " + zn + " - " +
                                                      "(" + d + " * " + "(" + zf + " - " + zn + ")));\n";
}

std::string LinearizeDepthSampleNode::GetInputDescription(unsigned int index) const
{
    return "Depth Sample";
}


void LinearizeDepthSampleNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 1,
           "Depth sample input must have size 1, not size " + ToString(GetInputs()[0].GetSize()));
}