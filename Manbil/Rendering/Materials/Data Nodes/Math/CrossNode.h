#pragma once

#include "../DataNode.h"


//A node that outputs the cross product of two vec3's.
class CrossNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;
    
    CrossNode(const DataLine & firstL, const DataLine & secondL, std::string name = "");

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual void AssertMyInputsValid(void) const override;

    ADD_NODE_REFLECTION_DATA_H(CrossNode)
};