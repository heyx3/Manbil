#pragma once

#include "../DataNode.h"


//Outputs the logarithm of an input node with a different input node's base.
class LogNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "logNode"; }

    LogNode(const DataLine & value, const DataLine & base)
        : DataNode(MakeVector(value, base), MakeVector(value.GetDataLineSize()))
    {
        Assert(base.GetDataLineSize() == 1, "base must have a size of 1! Instead it has size " + std::to_string(base.GetDataLineSize()));
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;
};