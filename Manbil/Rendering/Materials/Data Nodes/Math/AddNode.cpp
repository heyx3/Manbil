#include "AddNode.h"

AddNode::AddNode(const std::vector<DataLine> & toAdd)
    : DataNode(toAdd, MakeVector(toAdd[0].GetDataLineSize()))
{
    unsigned int size = toAdd[0].GetDataLineSize();
    for (unsigned int i = 1; i < toAdd.size(); ++i)
        Assert(toAdd[i].GetDataLineSize() == size,
               std::string() + "Input " + std::to_string(i + 1) +
               " isn't the same size as the first input -- size " + std::to_string(size));
}
AddNode::AddNode(DataLine toAdd1, DataLine toAdd2)
    : DataNode(MakeVector(toAdd1, toAdd2), MakeVector(toAdd1.GetDataLineSize()))
{
    Assert(toAdd1.GetDataLineSize() == toAdd2.GetDataLineSize(),
           "The two items to add must be the same size!");
}
AddNode::AddNode(DataLine toAdd1, DataLine toAdd2, DataLine toAdd3)
    : DataNode(MakeVector(toAdd1, toAdd2, toAdd3), MakeVector(toAdd1.GetDataLineSize()))
{
    Assert(toAdd1.GetDataLineSize() == toAdd2.GetDataLineSize(),
           "The first two items to add are not the same size!");
    Assert(toAdd1.GetDataLineSize() == toAdd3.GetDataLineSize(),
           "The third item to add is not the right size!");
}

void AddNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputs()[0]).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " + ";
        else outCode += ";\n";
    }
}