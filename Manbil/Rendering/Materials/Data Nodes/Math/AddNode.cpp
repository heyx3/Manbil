#include "AddNode.h"


MAKE_NODE_READABLE_CPP(AddNode, 0.0f, 0.0f)


unsigned int AddNode::GetOutputSize(unsigned int index) const
{
    return GetInputs()[0].GetSize();
}
std::string AddNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_added";
}

AddNode::AddNode(const std::vector<DataLine> & toAdd, std::string name)
    : DataNode(toAdd, name)
{
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

void AddNode::AssertMyInputsValid(void) const
{
    unsigned int size = GetInputs()[0].GetSize();
    for (unsigned int i = 1; i < GetInputs().size(); ++i)
        Assert(GetInputs()[i].GetSize() == size,
               "Input " + ToString(i + 1) + " isn't the same size as the first input -- size " + ToString(size));
}