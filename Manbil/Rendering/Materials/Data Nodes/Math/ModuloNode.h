#pragma once

#include "../DataNode.h"


//Outputs an input mod another input.
class ModuloNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "modulo"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    ModuloNode(const DataLine & numerator, const DataLine & divisor, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;
};