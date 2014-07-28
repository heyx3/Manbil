#include "AddNode.h"

AddNode::AddNode(const std::vector<DataLine> & toAdd, std::string name)
    : DataNode(toAdd, name)
{
    unsigned int size = toAdd[0].GetSize();
    for (unsigned int i = 1; i < toAdd.size(); ++i)
        Assert(toAdd[i].GetSize() == size,
               std::string() + "Input " + std::to_string(i + 1) +
               " isn't the same size as the first input -- size " + std::to_string(size));
}
AddNode::AddNode(DataLine toAdd1, DataLine toAdd2, std::string name)
    : DataNode(MakeVector(toAdd1, toAdd2), name)
{
    Assert(toAdd1.GetSize() == toAdd2.GetSize(),
           "The two items to add must be the same size!");
}
AddNode::AddNode(DataLine toAdd1, DataLine toAdd2, DataLine toAdd3, std::string name)
    : DataNode(MakeVector(toAdd1, toAdd2, toAdd3), name)
{
    Assert(toAdd1.GetSize() == toAdd2.GetSize(),
           "The first two items to add are not the same size!");
    Assert(toAdd1.GetSize() == toAdd3.GetSize(),
           "The third item to add is not the right size!");
}

void AddNode::ResetOutputs(std::vector<unsigned int> & newOuts) const
{
    newOuts.insert(newOuts.end(), GetInputs()[0].GetSize());
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