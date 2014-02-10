#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple division operator.
class DivideNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "divisionNode"; }

    DivideNode(DataLine baseValue, const std::vector<DataLine> & toDivide);
    DivideNode(DataLine baseValue, DataLine toDivide);

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;
};