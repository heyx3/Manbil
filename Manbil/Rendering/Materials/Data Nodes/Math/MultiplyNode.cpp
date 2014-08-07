#include "MultiplyNode.h"


MAKE_NODE_READABLE_CPP(MultiplyNode, 1.0f, 1.0f)


unsigned int GetMaxDLSize(const std::vector<DataLine> & lines)
{
    unsigned int max = 0;
    for (unsigned int i = 0; i < lines.size(); ++i)
        max = BasicMath::Max(max, lines[i].GetSize());
    return max;
}

unsigned int MultiplyNode::GetOutputSize(unsigned int index) const
{
    return GetMaxDLSize(GetInputs());
}
std::string MultiplyNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_multiplied";
}

MultiplyNode::MultiplyNode(const std::vector<DataLine> & toMultiply, std::string name)
    : DataNode(toMultiply, name)
{

}

void MultiplyNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " * ";
        else outCode += ";\n";
    }
}

void MultiplyNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs().size() > 0, "Need at least one input for this MultiplyNode!");

    unsigned int size = GetOutputSize(0);
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        Assert(GetInputs()[i].GetSize() == size || GetInputs()[i].GetSize() == 1,
               "Input #" + ToString(i) + " is not size 1 or " + ToString(size));
    }
}