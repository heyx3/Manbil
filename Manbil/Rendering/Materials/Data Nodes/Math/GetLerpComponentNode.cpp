#include "GetLerpComponentNode.h"


unsigned int GetLerpComponentNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetMinInput().GetSize();
}
std::string GetLerpComponentNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_lerpComponent";
}

GetLerpComponentNode::GetLerpComponentNode(const DataLine & min, const DataLine & max, const DataLine & value, std::string name)
    : DataNode(MakeVector(min, max, value),
               []() { return std::shared_ptr<DataNode>(new GetLerpComponentNode(DataLine(0.0f), DataLine(1.0f), DataLine(0.5f))); },
               name)
{
    Assert(min.GetSize() == max.GetSize(),
           std::string() + "'min' size is " + ToString(min.GetSize()) + " but 'max' size is " + ToString(max.GetSize()));
    Assert(value.GetSize() == min.GetSize() || value.GetSize() == 1,
           std::string() + "Value must be size 1 or " + ToString(min.GetSize()) + " but is it size " + ToString(value.GetSize()));
}

void GetLerpComponentNode::WriteMyOutputs(std::string & outCode) const
{
    std::string min = GetMinInput().GetValue(),
                max = GetMaxInput().GetValue(),
                value = GetValueInput().GetValue();
    outCode += "\t" + VectorF(GetMinInput().GetSize()).GetGLSLType() + " " + GetOutputName(0) + " = " +
                   "(" + value + " - " + min + ") / (" + max + " - " + min + ");\n";
}

std::string GetLerpComponentNode::GetInputDescription(unsigned int index) const
{
    return (index == 0 ? "Min" : (index == 1 ? "Max" : "Value"));
}