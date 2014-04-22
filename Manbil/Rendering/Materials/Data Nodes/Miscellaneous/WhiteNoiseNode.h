#pragma once

#include "../DataNode.h"


//A node that generates random noise given a seed input node.
class WhiteNoiseNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { "floorNode"; }

    WhiteNoiseNode(const DataLine & seed) : DataNode(makeInputs(seed), MakeVector(1)) { }

    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index));
        return GetName() + std::to_string(GetUniqueID()) + "_noise";
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    static std::vector<DataLine> makeInputs(const DataLine & seed);
};