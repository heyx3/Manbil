#include "DivideNode.h"

unsigned int GetMax(const std::vector<DataLine> & toDivide, const DataLine & baseValue)
{
    DataLine max = baseValue;

    for (int i = 0; i < toDivide.size(); ++i)
        if (toDivide[i].GetDataLineSize() > max.GetDataLineSize())
            max = toDivide[i];

    return max.GetDataLineSize();
}


DivideNode::DivideNode(DataLine baseValue, const std::vector<DataLine> & toDivide)
    : DataNode(MakeVector(baseValue, 0, toDivide), MakeVector(GetMax(toDivide, baseValue)))
{
    assert(toDivide.size() > 0);

    unsigned int size = GetOutputs()[0];
    for (unsigned int i = 0; i < toDivide.size(); ++i)
        assert(toDivide[i].GetDataLineSize() == size ||
               toDivide[i].GetDataLineSize() == 1);
}
DivideNode::DivideNode(DataLine toDivide1, DataLine toDivide2)
    : DataNode(MakeVector(toDivide1, toDivide2), MakeVector(BasicMath::Max(toDivide1.GetDataLineSize(), toDivide2.GetDataLineSize())))
{
    assert(toDivide1.GetDataLineSize() == toDivide2.GetDataLineSize() ||
           toDivide1.GetDataLineSize() == 1 ||
           toDivide2.GetDataLineSize() == 1);
}

void DivideNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = ";
    for (int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " / ";
        else outCode += ";\n";
    }
}