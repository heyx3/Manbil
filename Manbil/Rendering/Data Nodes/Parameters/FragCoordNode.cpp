#include "FragCoordNode.h"


ADD_NODE_REFLECTION_DATA_CPP_SINGLETON(FragCoordNode)


DataNode::Ptr FragCoordNode::instance = DataNode::Ptr(new FragCoordNode());

std::string FragCoordNode::GetOutputName(unsigned int outputIndex) const
{
    return "gl_FragCoord.xy";
}

void FragCoordNode::AssertMyInputsValid(void) const
{
    Assert(CurrentShader == SH_FRAGMENT, "Only works in fragment shader!");
}