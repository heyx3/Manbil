#include "SubtractNode.h"

SubtractNode::SubtractNode(DataLine baseValue, const std::vector<DataLine> & toSub)
: DataNode(MakeVector(baseValue, toSub.begin(), toSub), MakeVector(baseValue.GetDataLineSize()))
{
    unsigned int size = toSub[0].GetDataLineSize();
    for (unsigned int i = 0; i < toSub.size(); ++i)
        assert(toSub[i].GetDataLineSize() == size);
}
SubtractNode::SubtractNode(DataLine baseValuse, DataLine toSub)
: DataNode(MakeVector(baseValuse, toSub), MakeVector(baseValuse.GetDataLineSize()))
{
    assert(toAdd.GetDataLineSize() == toSub.GetDataLineSize());
}

void SubtractNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    std::string vecType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();

    outCode += "\t" + vecType + GetOutputName(0, shaderType) + " = ";
    for (int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue(shaderType);

        if (i < GetInputs().size() - 1) outCode += " - ";
        else outCode += ";\n";
    }
}