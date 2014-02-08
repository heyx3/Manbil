#pragma once

#include "DataNode.h"
#include "../MaterialData.h"

//The end of a series of data nodes, culminating in a channel value.
class ChannelNode : public DataNode
{
public:

    RenderingChannels Channel;
};