#include "SubtractNode.h"


DataNode::NodeFactory subtractNodeFactory = []() { return DataNodePtr(new SubtractNode(DataLine(VectorF(1.0f, 1.0f, 1.0f, 1.0f)), DataLine())); };


SubtractNode::SubtractNode(const DataLine & baseVal, const std::vector<DataLine> & toSubtract, std::string name)
    : DataNode(MakeVector(baseVal, 0, toSubtract), subtractNodeFactory, name)
{
    unsigned int size = baseVal.GetSize();

    for (unsigned int i = 0; i < toSubtract.size(); ++i)
        Assert(toSubtract[i].GetSize() == size,
               "Subtraction input " + ToString(i + 1) +
                   " is not the same size as the 'baseVal' input (" + ToString(size) + ")");
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