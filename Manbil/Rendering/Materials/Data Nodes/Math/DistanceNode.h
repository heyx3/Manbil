#pragma once

#include "../DataNode.h"


//Outputs the distance between two vectors.
class DistanceNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "distance"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    DistanceNode(const DataLine & pos1, const DataLine & pos2, std::string name = "");


protected:
    
    virtual void WriteMyOutputs(std::string & outCode) const override;
};