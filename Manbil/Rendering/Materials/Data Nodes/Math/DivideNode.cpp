#include "DivideNode.h"



unsigned int GetMax(const std::vector<DataLine> & toDivide, const DataLine & baseValue)
{
    DataLine max = baseValue;

    for (unsigned int i = 0; i < toDivide.size(); ++i)
        if (toDivide[i].GetSize() > max.GetSize())
            max = toDivide[i];

    return max.GetSize();
}


unsigned int DivideNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetMax(std::vector<DataLine>(GetInputs().begin() + 1, GetInputs().end()), GetInputs()[0]);
}
std::string DivideNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_divided";
}

DivideNode::DivideNode(DataLine baseValue, const std::vector<DataLine> & toDivide, std::string name)
    : DataNode(MakeVector(baseValue, 0, toDivide),
               []() { return std::shared_ptr<DataNode>(new DivideNode(DataLine(1.0f), DataLine(0.0f))); },
               name)
{
    Assert(toDivide.size() > 0, "'toDivide' vector must have at least one element!");

    unsigned int size = GetOutputSize(0);
    for (unsigned int i = 0; i < toDivide.size(); ++i)
        Assert(toDivide[i].GetSize() == size ||
               toDivide[i].GetSize() == 1,
               std::string() + "The " + ToString(i + 1) +
               "-th element in 'toDivide' doesn't have a size of 1 or " + ToString(size) + "!");
}

void DivideNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " / ";
        else outCode += ";\n";
    }
}