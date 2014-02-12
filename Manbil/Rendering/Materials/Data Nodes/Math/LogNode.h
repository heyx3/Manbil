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
        assert(base.GetDataLineSize() == 1);
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;
};