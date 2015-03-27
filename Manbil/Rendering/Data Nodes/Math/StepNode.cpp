#include "StepNode.h"


ADD_NODE_REFLECTION_DATA_CPP(StepNode, DataLine(), DataLine())


StepNode::StepNode(DataLine val, DataLine threshold, std::string name)
    : DataNode(MakeVector(val, threshold), name)
{

}

std::string StepNode::GetOutputName(unsigned int outputIndex) const
{
    return GetName() + "_stepped";
}
unsigned int StepNode::GetOutputSize(unsigned int outputIndex) const
{
    return Mathf::Max(GetInputs()[0].GetSize(), GetInputs()[1].GetSize());
}

std::string StepNode::GetInputDescription(unsigned int inIndex) const
{
    return (inIndex == 0) ? "Value to be stepped" : "Threshold";
}

void StepNode::AssertMyInputsValid(void) const
{
    unsigned int valSize = GetInputs()[0].GetSize(),
                 threshSize = GetInputs()[1].GetSize();
    Assert(threshSize == 1 || threshSize == valSize,
           "Threshold must be either size 1 or the same size as the value to step");
}
void StepNode::WriteMyOutputs(std::string& outCode) const
{
    outCode += "\t" + VectorF::GetGLSLType(GetOutputSize(0)) + " " + GetOutputName(0) +
                   " = step(" + GetInputs()[1].GetValue() + ", " + GetInputs()[0].GetValue() + ");\n";
}