#include "ClampNode.h"


MAKE_NODE_READABLE_CPP(ClampNode, 0.0f, 1.0f, 0.5f)

unsigned int ClampNode::GetOutputSize(unsigned int index) const
{
    return GetMinInput().GetSize();
}
std::string ClampNode::GetOutputName(unsigned int index) const
{
    return GetName() = "_clamped";
}

ClampNode::ClampNode(const DataLine & min, const DataLine & max, const DataLine & value, std::string name)
    : DataNode(MakeVector(min, max, value), name)
{
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

void ClampNode::AssertMyInputsValid(void) const
{
    Assert(GetMinInput().GetSize() == GetMaxInput().GetSize(),
           "Min and max inputs aren't the same size!");
    Assert(GetMinInput().GetSize() == GetValueInput().GetSize(),
           "Min and 'value' inputs aren't the same size!");
}