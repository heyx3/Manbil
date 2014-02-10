#include "AddNode.h"

AddNode::AddNode(const std::vector<DataLine> & toAdd)
    : DataNode(toAdd, MakeVector(toAdd[0].GetDataLineSize()))
{
    unsigned int size = toAdd[0].GetDataLineSize();
    for (unsigned int i = 0; i < toAdd.size(); ++i)
        assert(toAdd[i].GetDataLineSize() == size);
}
AddNode::AddNode(DataLine toAdd1, DataLine toAdd2)
    : DataNode(MakeVector(toAdd1, toAdd2), MakeVector(toAdd1.GetDataLineSize()))
{
    assert(toAdd1.GetDataLineSize() == toAdd2.GetDataLineSize());
}

void AddNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();

    outCode += "\t" + vecType + GetOutputName(0) + " = ";
    for (int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " + ";
        else outCode += ";\n";
    }
}