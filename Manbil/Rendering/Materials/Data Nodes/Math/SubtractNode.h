#pragma once

#include "../DataNode.h"


//Represents a simple subtraction operator.
class SubtractNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "subtraction"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetInputs()[0].GetSize();
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetName() + "_subtracted";
    }

    SubtractNode(const DataLine & baseValue, const std::vector<DataLine> & toSubtract, std::string name = "");
    SubtractNode(const DataLine & baseValue, const DataLine & toSubtract, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;
};