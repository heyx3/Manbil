#pragma once

#include "../DataNode.h"


//Outputs the max of an input.
class MaxNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    MaxNode(const DataLine & in1, const DataLine & in2, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual void AssertMyInputsValid(void) const override;

    MAKE_NODE_READABLE_H(MaxNode)
};

//Outputs the min of an input.
class MinNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    MinNode(const DataLine & in1, const DataLine & in2, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual void AssertMyInputsValid(void) const override;

    MAKE_NODE_READABLE_H(MinNode)
};