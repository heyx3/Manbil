#include "ModuloNode.h"


unsigned int ModuloNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetInputs()[0].GetSize();
}
std::string ModuloNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_result";
}

ModuloNode::ModuloNode(const DataLine & numerator, const DataLine & divisor, std::string name = "")
    : DataNode(MakeVector(numerator, divisor),
               []() { return DataNodePtr(new ModuloNode(DataLine(0.0f), DataLine(2.0f))); },
               name)
{
    Assert(numerator.GetSize() == divisor.GetSize() || divisor.GetSize() == 1,
           "Divisor must be size 1 or " + ToString(numerator.GetSize()) +
               ", but it is size " + ToString(divisor.GetSize()) + "!");
}

void ModuloNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetInputs()[0].GetSize()).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) +
                " = mod(" + GetInputs()[0].GetValue() + ", " +
                GetInputs()[1].GetValue() + ");\n";
}

std::string ModuloNode::GetInputDescription(unsigned int index) const
{
    return (index == 0) ? "Numerator" : "Denominator";
}