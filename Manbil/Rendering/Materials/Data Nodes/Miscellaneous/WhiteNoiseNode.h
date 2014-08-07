#pragma once

#include "../DataNode.h"


//A node that generates random noise given a seed input node.
//TODO: Create a PerlinNoiseNode class.
class WhiteNoiseNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    //The "randPeriodMultiplier" argument represents the quality of the noise.
    //A larger value yields more randomness.
    //A smaller value yields less flickering.
    WhiteNoiseNode(const DataLine & seed, std::string name = "", DataLine randPeriodMultiplier = DataLine(43758.5453f));


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void AssertMyInputsValid(void) const override;

    MAKE_NODE_READABLE_H(WhiteNoiseNode)
};