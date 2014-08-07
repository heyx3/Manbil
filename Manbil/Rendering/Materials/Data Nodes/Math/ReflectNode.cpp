#include "ReflectNode.h"


MAKE_NODE_READABLE_CPP(ReflectNode, Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f))


unsigned int ReflectNode::GetOutputSize(unsigned int index) const
{
    return 3;
}
std::string ReflectNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_reflected";
}

ReflectNode::ReflectNode(const DataLine & toReflect, const DataLine & reflectNormal, std::string name)
    : DataNode(MakeVector(toReflect, reflectNormal), name)
{
}

void ReflectNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\t" + VectorF(GetOutputSize(0)).GetGLSLType() + " " + GetOutputName(0) +
        " = reflect(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}

std::string ReflectNode::GetInputDescription(unsigned int index) const
{
    return (index == 0) ? "Vector to reflect" : "Reflection normal";
}

void ReflectNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 3, "'toReflect' input isn't size 3!");
    Assert(GetInputs()[1].GetSize() == 3, "'reflectNormal' input isn't size 3!");
}