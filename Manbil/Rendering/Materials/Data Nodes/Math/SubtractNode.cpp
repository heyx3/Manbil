#include "SubtractNode.h"


SubtractNode::SubtractNode(const DataLine & baseVal, const std::vector<DataLine> & toSubtract, std::string name)
    : DataNode(MakeVector(baseVal, 0, toSubtract), name)
{
    unsigned int size = baseVal.GetSize();
    for (unsigned int i = 0; i < toSubtract.size(); ++i)
        Assert(toSubtract[i].GetSize() == size,
               std::string() + "Subtraction input " + std::to_string(i + 1) +
               " is not the same size as the 'baseVal' input (" + std::to_string(size) + ")");
}
SubtractNode(const DataLine & baseValue, const DataLine & toSubtract, std::string name)
    : SubtractNode(baseVal, MakeVector(toSubtract), name)
{

}

void SubtractNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputs()[0]).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        if (i > 0) outCode += " - ";
        outCode += GetInputs()[i].GetValue();
    }
    outCode += ";\n";
}