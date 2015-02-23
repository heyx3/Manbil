#include "TangentSpaceNormalsNode.h"


ADD_NODE_REFLECTION_DATA_CPP(TangentSpaceNormalsNode, std::vector<DataLine>());


#pragma warning(disable: 4100)
std::string TangentSpaceNormalsNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_finalNormal";
}
unsigned int TangentSpaceNormalsNode::GetOutputSize(unsigned int index) const
{
    return 3;
}
#pragma warning(default: 4100)


void TangentSpaceNormalsNode::AssertMyInputsValid(void) const
{
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
        Assert(GetInputs()[i].GetSize() == 3,
               "Input normal " + ToString(i) + " isn't size 3!");
}

void TangentSpaceNormalsNode::WriteMyOutputs(std::string& outCode) const
{
    if (GetInputs().size() == 0)
    {
        outCode += "\tvec3 " + GetOutputName(0) + " = vec3(0.0f, 0.0f, 1.0f);\n";
    }
    else
    {
        outCode += "\tvec3 " + GetOutputName(0) + " = normalize(" + GetInputs()[0].GetValue();
        for (unsigned int i = 1; i < GetInputs().size(); ++i)
            outCode += " + " + GetInputs()[i].GetValue();
        outCode += ");\n";
    }
}

std::string TangentSpaceNormalsNode::GetInputDescription(unsigned int index) const
{
    return "inNormal" + ToString(index + 1);
}