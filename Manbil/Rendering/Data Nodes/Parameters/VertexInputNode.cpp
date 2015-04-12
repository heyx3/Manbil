#include "VertexInputNode.h"


ADD_NODE_REFLECTION_DATA_CPP_SINGLETON(VertexInputNode)


std::shared_ptr<DataNode> VertexInputNode::instance = std::shared_ptr<DataNode>(new VertexInputNode());


unsigned int VertexInputNode::GetOutputSize(unsigned int outputIndex) const
{
    return GetMatData()->VertexInputs.GetAttribute(outputIndex).Size;
}
std::string VertexInputNode::GetOutputName(unsigned int outputIndex) const
{
    return GetMatData()->VertexInputs.GetAttribute(outputIndex).Name;
}


#pragma warning(disable: 4100)
void VertexInputNode::WriteMyOutputs(std::string & outCode) const
{
    Assert(CurrentShader == SH_VERTEX,
           std::string("Invalid shader type (must be Vertex): ") + ToString(CurrentShader));
    //Don't actually output anything, since the output name is an "in" variable.
}
#pragma warning(default: 4100)


VertexInputNode::VertexInputNode(void) : DataNode(std::vector<DataLine>(), GetInstanceName()) { }
