#include "ApplyNormalMapNode.h"


ADD_NODE_REFLECTION_DATA_CPP(ApplyNormalMapNode, Vector3f(), Vector3f(), Vector3f(), Vector3f())


#pragma warning(disable: 4100)
unsigned int ApplyNormalMapNode::GetOutputSize(unsigned int index) const
{
    return 3;
}
std::string ApplyNormalMapNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_worldNormal";
}
#pragma warning(default: 4100)

ApplyNormalMapNode::ApplyNormalMapNode(const DataLine& normal, const DataLine& tangent,
                                       const DataLine& bitangent, const DataLine& tangentSpaceNormal,
                                       std::string name)
    : DataNode(MakeVector(normal, tangent, bitangent, tangentSpaceNormal), name)
{

}

void ApplyNormalMapNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 3, "Surface normal must be size 3");
    Assert(GetInputs()[1].GetSize() == 3, "Surface tangent must be size 3");
    Assert(GetInputs()[2].GetSize() == 3, "Surface bitangent must be size 3");
    Assert(GetInputs()[3].GetSize() == 3, "Tangent-space normal must be size 3");
}

void ApplyNormalMapNode::WriteMyOutputs(std::string& outCode) const
{
    outCode += "\tmat3 " + GetName() + "_transf = mat3(" + GetInputs()[1].GetValue() + ", " +
                                                           GetInputs()[2].GetValue() + ", " +
                                                           GetInputs()[0].GetValue() + ");\n";
    outCode += "\tvec3 " + GetOutputName(0) + " = normalize(" + GetName() + "_transf * " + GetInputs()[3].GetValue() + ");\n";
}