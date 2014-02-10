#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple subtraction operator.
class SubtractNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "additionNode"; }

    SubtractNode(DataLine baseValue, const std::vector<DataLine> & toSubtract);
    SubtractNode(DataLine baseValue, DataLine toSubtract);

    virtual void WriteMyOutputs(std::string & outCode) const override;
};