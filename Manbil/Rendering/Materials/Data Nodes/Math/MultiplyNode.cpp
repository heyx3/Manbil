#include "MultiplyNode.h"

MultiplyNode::MultiplyNode(const std::vector<DataLine> & toMultiply)
: DataNode(toMultiply, MakeVector(toMultiply[0].GetDataLineSize()))
{
    unsigned int size = toMultiply[0].GetDataLineSize();
    for (unsigned int i = 0; i < toMultiply.size(); ++i)
        assert(toMultiply[i].GetDataLineSize() == size ||
               toMultiply[i].GetDataLineSize() == 1);
}
MultiplyNode::MultiplyNode(DataLine toMultiply1, DataLine toMultiply2)
: DataNode(MakeVector(toMultiply1, toMultiply2), MakeVector(toMultiply1.GetDataLineSize()))
{
    assert(toMultiply1.GetDataLineSize() == toMultiply2.GetDataLineSize() ||
           toMultiply1.GetDataLineSize() == 1 ||
           toMultiply2.GetDataLineSize() == 1);
}

void MultiplyNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    std::string vecType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();

    outCode += "\t" + vecType + GetOutputName(0, shaderType) + " = ";
    for (int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " * ";
        else outCode += ";\n";
    }
}