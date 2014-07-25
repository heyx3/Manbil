#include "MultiplyNode.h"

unsigned int GetMax(const std::vector<DataLine> & lines)
{
    if (lines.size() == 0) return 0;

    unsigned int max = lines[0].GetDataLineSize();
    for (unsigned int i = 1; i < lines.size(); ++i)
    {
        max = BasicMath::Max(max, lines[i].GetDataLineSize());
    }
    return max;
}

MultiplyNode::MultiplyNode(const std::vector<DataLine> & toMultiply)
: DataNode(toMultiply, MakeVector(GetMax(toMultiply)))
{
    Assert(toMultiply.size() > 0, "Need at least one input for this MultiplyNode!");

    unsigned int size = GetOutputs()[0];
    for (unsigned int i = 0; i < toMultiply.size(); ++i)
    {
        Assert(toMultiply[i].GetDataLineSize() == size || toMultiply[i].GetDataLineSize() == 1,
               "Input #" + std::to_string(i) + " is not size 1 or " + std::to_string(size));
    }
}
MultiplyNode::MultiplyNode(DataLine toMultiply1, DataLine toMultiply2)
: DataNode(MakeVector(toMultiply1, toMultiply2), MakeVector(BasicMath::Max(toMultiply1.GetDataLineSize(), toMultiply2.GetDataLineSize())))
{
    unsigned int size1 = toMultiply1.GetDataLineSize(),
                 size2 = toMultiply2.GetDataLineSize();
    Assert(size1 == size2 || size1 == 1 || size2 == 1,
           "The first multiply input is size " + std::to_string(size1) + ", and the second input is size " + std::to_string(size2) + "!");
}

void MultiplyNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputs()[0]).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " * ";
        else outCode += ";\n";
    }
}