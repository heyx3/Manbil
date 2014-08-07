#pragma once

#include "../DataNode.h"


//Outputs the distance between two vectors.
class DistanceNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    DistanceNode(const DataLine & pos1, const DataLine & pos2, std::string name = "");


protected:
    
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual void AssertMyInputsValid(void) const override;

    MAKE_NODE_READABLE_H(DistanceNode)
};