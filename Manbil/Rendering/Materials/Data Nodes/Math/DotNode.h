#pragma once

#include "../DataNode.h"


//Outputs the dot product of two input nodes.
class DotNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "dotProduct"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    DotNode(const DataLine & first, const DataLine & second, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;
};