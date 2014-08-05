#include "LinearizeDepthSampleNode.h"


unsigned int LinearizeDepthSampleNode::GetOutputSize(unsigned int i) const
{
    Assert(i == 0, "Invalid output index " + ToString(i));
    return 1;
}
std::string LinearizeDepthSampleNode::GetOutputName(unsigned int i) const
{
    Assert(i == 0, "Invalid output index " + ToString(i));
    return GetName() + "_linearized";
}

LinearizeDepthSampleNode::LinearizeDepthSampleNode(const DataLine & dIn, std::string name)
    : DataNode(MakeVector(dIn),
               []() { return Ptr(new LinearizeDepthSampleNode(DataLine(VectorF(0.5f)))); },
               name)
{
    Assert(dIn.GetSize() == 1, "Depth sample input must have size 1, not size " + ToString(dIn.GetSize()));
}

void LinearizeDepthSampleNode::SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
{
    flags.EnableFlag(MaterialUsageFlags::DNF_USES_ZNEAR);
    flags.EnableFlag(MaterialUsageFlags::DNF_USES_ZFAR);
}
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