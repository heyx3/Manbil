#include "AddNode.h"


unsigned int AddNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetInputs()[0].GetSize();
}
std::string AddNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_added";
}

AddNode::AddNode(const std::vector<DataLine> & toAdd, std::string name)
    : DataNode(toAdd,
               []() { return std::shared_ptr<DataNode>(new AddNode(DataLine(1.0f), DataLine(0.0f))); },
               name)
{
    unsigned int size = toAdd[0].GetSize();
    for (unsigned int i = 1; i < toAdd.size(); ++i)
        Assert(toAdd[i].GetSize() == size,
               "Input " + ToString(i + 1) + " isn't the same size as the first input -- size " + ToString(size));
}

void AddNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " + ";
        else outCode += ";\n";
    }
}