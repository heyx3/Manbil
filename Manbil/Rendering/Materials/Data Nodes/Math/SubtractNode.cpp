#include "SubtractNode.h"


MAKE_NODE_READABLE_CPP(SubtractNode, 1.0f, 0.0f)


SubtractNode::SubtractNode(const DataLine & baseVal, const std::vector<DataLine> & toSubtract, std::string name)
    : DataNode(MakeVector(baseVal, 0, toSubtract), name)
{

}

void SubtractNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        if (i > 0) outCode += " - ";
        outCode += GetInputs()[i].GetValue();
    }
    outCode += ";\n";
}

std::string SubtractNode::GetInputDescription(unsigned int index) const
{
    return (index == 0 ? "Base Value" : "Subtraction " + ToString(index));
}

void SubtractNode::AssertMyInputsValid(void) const
{
    unsigned int size = GetInputs()[0].GetSize();

    for (unsigned int i = 0; i < GetInputs().size(); ++i)
        Assert(GetInputs()[i].GetSize() == size,
               "Subtraction input " + ToString(i + 1) +
                   " is not the same size as the 'baseVal' input (" + ToString(size) + ")");
}