#include "ModuloNode.h"


ADD_NODE_REFLECTION_DATA_CPP(ModuloNode, 1.0f, 2.0f)


#pragma warning(disable: 4100)
unsigned int ModuloNode::GetOutputSize(unsigned int index) const
{
    return GetInputs()[0].GetSize();
}
std::string ModuloNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_result";
}
#pragma warning(default: 4100)

ModuloNode::ModuloNode(const DataLine & numerator, const DataLine & divisor, std::string name)
    : DataNode(MakeVector(numerator, divisor), name)
{

}

void ModuloNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetInputs()[0].GetSize(), 0).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) +
                " = mod(" + GetInputs()[0].GetValue() + ", " +
                GetInputs()[1].GetValue() + ");\n";
}

std::string ModuloNode::GetInputDescription(unsigned int index) const
{
    return (index == 0) ? "Numerator" : "Denominator";
}

void ModuloNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == GetInputs()[1].GetSize() || GetInputs()[0].GetSize() == 1,
           "Divisor must be size 1 or " + ToString(GetInputs()[0].GetSize()) +
               ", but it is size " + ToString(GetInputs()[0].GetSize()) + "!");
}