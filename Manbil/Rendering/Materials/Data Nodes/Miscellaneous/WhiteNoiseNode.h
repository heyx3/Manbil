#pragma once

#include "../DataNode.h"


//A node that generates random noise given a seed input node.
//TODO: Create a PerlinNoiseNode class.
class WhiteNoiseNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "whiteNoiseNode"; }

    //The "randPeriodMultiplier" argument represents the quality of the noise.
    //A larger value yields more randomness.
    //A smaller value yields less flickering.
    WhiteNoiseNode(const DataLine & seed, DataLine randPeriodMultiplier = DataLine(43758.5453f)) : DataNode(makeInputs(seed, randPeriodMultiplier), MakeVector(1)) { }

    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index));
        return GetName() + std::to_string(GetUniqueID()) + "_noise";
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    static std::vector<DataLine> makeInputs(const DataLine & seed, DataLine randPeriodMultiplier);
};