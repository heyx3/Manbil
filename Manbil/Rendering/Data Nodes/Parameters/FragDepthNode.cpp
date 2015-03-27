#include "FragDepthNode.h"


ADD_NODE_REFLECTION_DATA_CPP_SINGLETON(FragDepthNode)


DataNode::Ptr FragDepthNode::instance = DataNode::Ptr(new FragDepthNode());

std::string FragDepthNode::GetOutputName(unsigned int outputIndex) const
{
    return "gl_FragCoord.z";
}

void FragDepthNode::AssertMyInputsValid(void) const
{
    Assert(CurrentShader == SH_FRAGMENT, "Only works in fragment shader!");
}