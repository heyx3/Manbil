#include "VertexInputNode.h"



DataNodePtr VertexInputNode::instance = DataNodePtr(new VertexInputNode());


unsigned int VertexInputNode::GetOutputSize(unsigned int outputIndex) const
{
    Assert(outputIndex < VertexIns.GetNumbAttributes(),
           std::string() + "Invalid attribute number " + ToString(outputIndex) +
           ", must be less than " + ToString(VertexIns.GetNumbAttributes()));
    Assert(CurrentShader == ShaderHandler::SH_Vertex_Shader,
           std::string() + "Invalid shader type (must be Vertex): " + ToString(CurrentShader));
    return VertexIns.GetAttributeSize(outputIndex);
}
std::string VertexInputNode::GetOutputName(unsigned int outputIndex) const
{
    Assert(outputIndex < VertexIns.GetNumbAttributes(),
           std::string() + "Invalid attribute number " + ToString(outputIndex) +
           ", must be less than " + ToString(VertexIns.GetNumbAttributes()));
    Assert(CurrentShader == ShaderHandler::SH_Vertex_Shader,
           std::string() + "Invalid shader type (must be Vertex): " + ToString(CurrentShader));
    return VertexIns.GetAttributeName(outputIndex);
}


#pragma warning(disable: 4100)

void VertexInputNode::WriteMyOutputs(std::string & outCode) const
{
    Assert(CurrentShader == ShaderHandler::SH_Vertex_Shader,
           std::string() + "Invalid shader type (must be Vertex): " + ToString(CurrentShader));
    //Don't actually output anything, since the output name is an "in" variable.
}
#pragma warning(default: 4100)

VertexInputNode::VertexInputNode(void) : DataNode(std::vector<DataLine>(), []() { return instance; }, "vertexIns") { }
