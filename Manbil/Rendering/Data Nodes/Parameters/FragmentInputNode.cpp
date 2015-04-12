#include "FragmentInputNode.h"


ADD_NODE_REFLECTION_DATA_CPP_SINGLETON(FragmentInputNode)


std::shared_ptr<DataNode> FragmentInputNode::instance = std::shared_ptr<DataNode>(new FragmentInputNode());

unsigned int FragmentInputNode::GetNumbOutputs(void) const
{
    if (GetMatData()->GeoShader.IsValidData())
        return GetMatData()->GeoShader.OutputTypes.GetNumbAttributes();
    else return GetMatData()->MaterialOuts.VertexOutputs.size();
}

unsigned int FragmentInputNode::GetOutputSize(unsigned int index) const
{
    return GetMatData()->GeoShader.IsValidData() ?
               GetMatData()->GeoShader.OutputTypes.GetAttribute(index).Size :
               GetMatData()->MaterialOuts.VertexOutputs[index].Value.GetSize();
}
std::string FragmentInputNode::GetOutputName(unsigned int index) const
{
    return GetMatData()->GeoShader.IsValidData() ?
               GetMatData()->GeoShader.OutputTypes.GetAttribute(index).Name :
               GetMatData()->MaterialOuts.VertexOutputs[index].Name;
}

#pragma warning(disable: 4100)
void FragmentInputNode::WriteMyOutputs(std::string & outCode) const
{
    Assert(CurrentShader == SH_FRAGMENT,
           std::string("Invalid shader type (must be Fragment): ") + ToString(CurrentShader));
    //Don't actually do anything, since the output is just an "in" variable.
}
#pragma warning(default: 4100)


FragmentInputNode::FragmentInputNode(void) : DataNode(std::vector<DataLine>(), GetInstanceName()) { }