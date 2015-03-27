#pragma once

#include "../DataNode.h"


//Outputs 0 if a given value is less than a given threshold. Otherwise, outputs 1.
class StepNode : public DataNode
{
public:

    StepNode(DataLine value, DataLine threshold, std::string name = "");


    virtual std::string GetOutputName(unsigned int outputIndex) const override;
    virtual unsigned int GetOutputSize(unsigned int outputIndex) const override;


protected:

    virtual void AssertMyInputsValid(void) const override;
    virtual void WriteMyOutputs(std::string& outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;


    ADD_NODE_REFLECTION_DATA_H(StepNode)
};