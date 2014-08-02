#include "TimeNode.h"


DataNodePtr TimeNode::instance = DataNodePtr(new TimeNode());


unsigned int TimeNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
    return 1;
}
std::string TimeNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
    return MaterialConstants::ElapsedTimeName;
}

void TimeNode::SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
{
    flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_TIME);
}
void TimeNode::WriteMyOutputs(std::string & outCode) const
{
    //No outputting needed.
}


TimeNode::TimeNode(void) : DataNode(std::vector<DataLine>(), []() { return instance; }, "elapsedTimeUniform") { }