#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple subtraction operator.
class SubtractNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "subtractionNode"; }

    SubtractNode(DataLine baseValue, const std::vector<DataLine> & toSubtract);
    SubtractNode(DataLine baseValue, DataLine toSubtract);


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;
};