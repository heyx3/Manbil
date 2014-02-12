#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple subtraction operator.
class SubtractNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "subtractionNode"; }

    SubtractNode(const DataLine & baseValue, const std::vector<DataLine> & toSubtract)
        : DataNode(MakeVector(baseValue, toSubtract.begin(), toSubtract), MakeVector(baseValue.GetDataLineSize()))
    {
        unsigned int size = toSubtract[0].GetDataLineSize();
        for (unsigned int i = 0; i < toSubtract.size(); ++i)
            assert(toSubtract[i].GetDataLineSize() == size);
    }
    SubtractNode(const DataLine & baseValue, const DataLine & toSubtract)
        : DataNode(MakeVector(baseValue, toSubtract), MakeVector(baseValue.GetDataLineSize()))
    {
        assert(baseValue.GetDataLineSize() == toSubtract.GetDataLineSize());
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;
};