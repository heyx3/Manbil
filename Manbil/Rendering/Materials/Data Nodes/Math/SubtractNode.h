#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"

//Represents a simple subtraction operator.
class SubtractNode : public DataNode
{
public:

    SubtractNode(const DataLine & baseValue, const std::vector<DataLine> & toSubtract, std::string name = "");
    SubtractNode(const DataLine & baseValue, const DataLine & toSubtract, std::string name = "");

protected:

    virtual void ResetOutputs(std::vector<unsigned int> & newOuts) const override
    {
        newOuts.insert(newOuts.end(), GetInputs()[0].GetSize());
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};