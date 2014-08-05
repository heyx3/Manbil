#include "MultiplyNode.h"



unsigned int GetMaxDLSize(const std::vector<DataLine> & lines)
{
    unsigned int max = 0;
    for (unsigned int i = 0; i < lines.size(); ++i)
        max = BasicMath::Max(max, lines[i].GetSize());
    return max;
}

unsigned int MultiplyNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetMaxDLSize(GetInputs());
}
std::string MultiplyNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_multiplied";
}

MultiplyNode::MultiplyNode(const std::vector<DataLine> & toMultiply, std::string name)
    : DataNode(toMultiply,
               []() { return Ptr(new MultiplyNode(DataLine(1.0f), DataLine(1.0f))); },
               name)
{
    Assert(toMultiply.size() > 0, "Need at least one input for this MultiplyNode!");

    unsigned int size = GetOutputSize(0);
    for (unsigned int i = 0; i < toMultiply.size(); ++i)
    {
        Assert(toMultiply[i].GetSize() == size || toMultiply[i].GetSize() == 1,
               "Input #" + ToString(i) + " is not size 1 or " + std::to_string(size));
    }
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