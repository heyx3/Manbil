#pragma once

#include "../DataNode.h"


//A node that generates random noise given a seed input node.
class WhiteNoiseNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { "floorNode"; }

    WhiteNoiseNode(const DataLine & seed) : DataNode(makeInputs(seed), MakeVector(1)) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;

private:

    static std::vector<DataLine> makeInputs(const DataLine & seed);
};