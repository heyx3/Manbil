#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"

//TODO: Pull into .cpp.

//Represents a simple subtraction operator.
class SubtractNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "subtractionNode"; }

    SubtractNode(const DataLine & baseValue, const std::vector<DataLine> & toSubtract)
        : DataNode(MakeVector(baseValue, 0, toSubtract), MakeVector(baseValue.GetDataLineSize()))
    {
        unsigned int size = baseValue.GetDataLineSize();
        for (unsigned int i = 0; i < toSubtract.size(); ++i)
            Assert(toSubtract[i].GetDataLineSize() == size,
                   std::string() + "Element " + std::to_string(i + 1) +
                   " is not the same size as the first element -- size " + std::to_string(size));
    }
    SubtractNode(const DataLine & baseValue, const DataLine & toSubtract)
        : DataNode(MakeVector(baseValue, toSubtract), MakeVector(baseValue.GetDataLineSize()))
    {
        Assert(baseValue.GetDataLineSize() == toSubtract.GetDataLineSize(),
               "The two inputs must be the same size!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;
};