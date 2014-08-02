#include "ReflectNode.h"


unsigned int ReflectNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return 3;
}
std::string ReflectNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_reflected";
}

ReflectNode::ReflectNode(const DataLine & toReflect, const DataLine & reflectNormal, std::string name = "")
    : DataNode(MakeVector(toReflect, reflectNormal),
               []() { return DataNodePtr(new ReflectNode(DataLine(VectorF(1.0f, 0.0f, 0.0f)), DataLine(VectorF(0.0f, 0.0f, 1.0f)))); },
               name)
{
    Assert(toReflect.GetSize() == 3, "'toReflect' isn't size 3!");
    Assert(reflectNormal.GetSize() == 3, "'reflectNormal' isn't size 3!");
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