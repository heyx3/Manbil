#include "GetLerpComponentNode.h"


ADD_NODE_REFLECTION_DATA_CPP(GetLerpComponentNode, 0.0f, 1.0f, 0.5f)


#pragma warning(disable: 4100)
unsigned int GetLerpComponentNode::GetOutputSize(unsigned int index) const
{
    return GetMinInput().GetSize();
}
std::string GetLerpComponentNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_lerpComponent";
}
#pragma warning(default: 4100)

GetLerpComponentNode::GetLerpComponentNode(const DataLine & min, const DataLine & max, const DataLine & value, std::string name)
    : DataNode(MakeVector(min, max, value), name)
{
}

void GetLerpComponentNode::WriteMyOutputs(std::string & outCode) const
{
    std::string min = GetMinInput().GetValue(),
                max = GetMaxInput().GetValue(),
                value = GetValueInput().GetValue();
    outCode += "\t" + VectorF(GetMinInput().GetSize(), 0).GetGLSLType() + " " + GetOutputName(0) + " = " +
                   "(" + value + " - " + min + ") / (" + max + " - " + min + ");\n";
}

std::string GetLerpComponentNode::GetInputDescription(unsigned int index) const
{
    return (index == 0 ? "Min" : (index == 1 ? "Max" : "Value"));
}

void GetLerpComponentNode::AssertMyInputsValid(void) const
{
    Assert(GetMinInput().GetSize() == GetMaxInput().GetSize(),
           "'min' size is " + ToString(GetMinInput().GetSize()) +
               " but 'max' input size is " + ToString(GetMaxInput().GetSize()));
    Assert(GetValueInput().GetSize() == GetMinInput().GetSize() || GetValueInput().GetSize() == 1,
           "'value' input must be size 1 or " + ToString(GetMinInput().GetSize()) +
               " but is it size " + ToString(GetValueInput().GetSize()));
}