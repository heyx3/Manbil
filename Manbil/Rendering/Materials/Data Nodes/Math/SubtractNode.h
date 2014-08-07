#pragma once

#include "../DataNode.h"


//Represents a simple subtraction operator.
class SubtractNode : public DataNode
{
public:

#pragma warning(disable: 4100)
    virtual unsigned int GetOutputSize(unsigned int index) const override { return GetInputs()[0].GetSize(); }
    virtual std::string GetOutputName(unsigned int index) const override { return GetName() + "_subtracted"; }
#pragma warning(default: 4100)

    SubtractNode(const DataLine & baseValue, const std::vector<DataLine> & toSubtract, std::string name = "");
    SubtractNode(const DataLine & baseValue, const DataLine & toSubtract, std::string name = "") : SubtractNode(baseValue, MakeVector(toSubtract), name) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void AssertMyInputsValid(void) const override;


    MAKE_NODE_READABLE_H(SubtractNode)
};