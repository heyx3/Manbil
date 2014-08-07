#include "FragmentInputNode.h"


MAKE_NODE_READABLE_CPP(FragmentInputNode, )


std::shared_ptr<DataNode> FragmentInputNode::instance = std::shared_ptr<DataNode>(new FragmentInputNode());

unsigned int FragmentInputNode::GetNumbOutputs(void) const
{
    if (GeometryShader.IsValidData())
        return GeometryShader.OutputTypes.GetNumbAttributes();
    else return MaterialOuts.VertexOutputs.size();
}

unsigned int FragmentInputNode::GetOutputSize(unsigned int index) const
{
    return GeometryShader.IsValidData() ?
               GeometryShader.OutputTypes.GetAttributeSize(index) :
               MaterialOuts.VertexOutputs[index].Value.GetSize();
}
std::string FragmentInputNode::GetOutputName(unsigned int index) const
{
    return GeometryShader.IsValidData() ?
               GeometryShader.OutputTypes.GetAttributeName(index) :
               MaterialOuts.VertexOutputs[index].Name;
}

#pragma warning(disable: 4100)
void FragmentInputNode::WriteMyOutputs(std::string & outCode) const
{
    Assert(CurrentShader == ShaderHandler::SH_Fragment_Shader,
           std::string() + "Invalid shader type (must be Fragment): " + ToString(CurrentShader));
    //Dont' actually do anything, since the output is just an "in" variable.
}
#pragma warning(default: 4100)


FragmentInputNode::FragmentInputNode(void) : DataNode(std::vector<DataLine>(), GetInstanceName())
{ }