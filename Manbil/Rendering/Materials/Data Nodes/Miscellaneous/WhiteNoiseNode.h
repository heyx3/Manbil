#pragma once

#include "../DataNode.h"


//A node that generates random noise given a seed input node.
//TODO: Create a PerlinNoiseNode class.
class WhiteNoiseNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "whiteNoise"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return 1;
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetName() + "_noiseVal";
    }

    //The "randPeriodMultiplier" argument represents the quality of the noise.
    //A larger value yields more randomness.
    //A smaller value yields less flickering.
    WhiteNoiseNode(const DataLine & seed, std::string name = "", DataLine randPeriodMultiplier = DataLine(43758.5453f))
        : DataNode(MakeVector(seed, randPeriodMultiplier),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new WhiteNoiseNode(ins[0], _name, ins[1])); },
                   name)
    {
        Assert(seed.GetSize() == randPeriodMultiplier.GetSize(),
               std::string() + "Seed and 'period multiplier' must be the same size!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        Assert(index <= 1, "Invalid output index " + ToString(index));
        return (index == 0 ? "Seed" : "RandPeriodMultiplier");
    }
};