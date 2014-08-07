#include "DivideNode.h"


MAKE_NODE_READABLE_CPP(DivideNode, 1.0f, 1.0f)


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
    return GetMax(std::vector<DataLine>(GetInputs().begin() + 1, GetInputs().end()), GetInputs()[0]);
}
std::string DivideNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_divided";
}

DivideNode::DivideNode(DataLine baseValue, const std::vector<DataLine> & toDivide, std::string name)
    : DataNode(MakeVector(baseValue, 0, toDivide), name)
{
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

void DivideNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs().size() > 0, "'toDivide' vector must have at least one element!");

    unsigned int size = GetOutputSize(0);

    for (unsigned int i = 0; i < GetInputs().size(); ++i)
        Assert(GetInputs()[i].GetSize() == size ||
                  GetInputs()[i].GetSize() == 1,
               "The " + ToString(i + 1) +
                  "-th input doesn't have a size of 1 or " + ToString(size) + "!");
}