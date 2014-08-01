#pragma once

#include "../DataNode.h"


//Raises an input to the power of another input.
class PowNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "pow"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    const DataLine & GetBaseInput(void) const { return GetInputs()[0]; }
    const DataLine & GetExponentInput(void) const { return GetInputs()[1]; }

    PowNode(const DataLine & base, const DataLine & exponent, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;
};