#include "ClampNode.h"


unsigned int ClampNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetMinInput().GetSize();
}
std::string ClampNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() = "_clamped";
}

ClampNode::ClampNode(const DataLine & min, const DataLine & max, const DataLine & value, std::string name)
    : DataNode(MakeVector(min, max, value),
               []() { return DataNodePtr(new ClampNode(DataLine(0.0f), DataLine(1.0f), DataLine(0.5f))); },
               name)
{
    Assert(min.GetSize() == max.GetSize(),
           "Min and max value aren't the same size!");
    Assert(min.GetSize() == value.GetSize(),
           "Min/max and value to clamp aren't the same size!");
}

void ClampNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
    outCode += "\t" + vecType + " " + GetOutputName(0) +
                    " = clamp(" + GetValueInput().GetValue() + ", " +
                    GetMinInput().GetValue() + ", " +
                    GetMaxInput().GetValue() + ");\n";
}

std::string ClampNode::GetInputDescription(unsigned int index) const
{
    return (index == 0 ? "Min" : (index == 1 ? "Max" : "Value to clamp"));
}