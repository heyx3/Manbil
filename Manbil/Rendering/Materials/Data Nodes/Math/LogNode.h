#pragma once

#include "../DataNode.h"


//Outputs the logarithm of an input node with a different input node's base.
class LogNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    LogNode(const DataLine & value, DataLine base = DataLine(VectorF(2.0f)), std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void AssertMyInputsValid(void) const override;


    MAKE_NODE_READABLE_H(LogNode)
};