#include "MultiplyNode.h"

unsigned int GetMax(const std::vector<DataLine> & lines)
{
    unsigned int max = lines[0].GetDataLineSize();
    for (int i = 1; i < lines.size(); ++i)
    {
        max = BasicMath::Max(max, lines[i].GetDataLineSize());
    }
    return max;
}

MultiplyNode::MultiplyNode(const std::vector<DataLine> & toMultiply)
: DataNode(toMultiply, MakeVector(GetMax(toMultiply)))
{
    unsigned int size = GetOutputs()[0];
    for (unsigned int i = 0; i < toMultiply.size(); ++i)
        assert(toMultiply[i].GetDataLineSize() == size ||
               toMultiply[i].GetDataLineSize() == 1);
}
MultiplyNode::MultiplyNode(DataLine toMultiply1, DataLine toMultiply2)
: DataNode(MakeVector(toMultiply1, toMultiply2), MakeVector(BasicMath::Max(toMultiply1.GetDataLineSize(), toMultiply2.GetDataLineSize())))
{
    assert(toMultiply1.GetDataLineSize() == toMultiply2.GetDataLineSize() ||
           toMultiply1.GetDataLineSize() == 1 ||
           toMultiply2.GetDataLineSize() == 1);
}

void MultiplyNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " * ";
        else outCode += ";\n";
    }
}