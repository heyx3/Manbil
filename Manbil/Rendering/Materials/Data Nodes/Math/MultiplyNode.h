#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple multiplication operator.
class MultiplyNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "MultiplyitionNode"; }

    MultiplyNode(const std::vector<DataLine> & toMultiply);
    MultiplyNode(DataLine toMultiply1, DataLine toMultiply2);

    virtual void WriteMyOutputs(std::string & outCode) const override;
};