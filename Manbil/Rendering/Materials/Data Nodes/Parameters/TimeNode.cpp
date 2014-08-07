#include "TimeNode.h"


MAKE_NODE_READABLE_CPP(TimeNode, )

std::shared_ptr<DataNode> TimeNode::instance = std::shared_ptr<DataNode>(new TimeNode());


unsigned int TimeNode::GetOutputSize(unsigned int index) const
{
    return 1;
}
std::string TimeNode::GetOutputName(unsigned int index) const
{
    return MaterialConstants::ElapsedTimeName;
}

#pragma warning(disable: 4100)
void TimeNode::SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
{
    flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_TIME);
}

void TimeNode::WriteMyOutputs(std::string & outCode) const
{
    //No outputting needed.
}
#pragma warning(default: 4100)



TimeNode::TimeNode(void) : DataNode(std::vector<DataLine>(), GetInstanceName()){ }