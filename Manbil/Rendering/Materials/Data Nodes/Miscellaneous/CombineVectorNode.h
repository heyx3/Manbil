#pragma once

#include "../DataNode.h"


//Combines multiple smaller floats/vectors into one large vector.
class CombineVectorNode : public DataNode
{
public:

    virtual std::string GetName(void) const { return "combineVectorNode"; }

    CombineVectorNode(const std::vector<DataLine> & inputs) : DataNode(inputs, MakeVector(CountElements(inputs)))
    {
        count = GetOutputs()[0];
        Assert(count > 0, std::string() + "No elements in the output vector!");
        Assert(count < 5, std::string() + "Too many elements in the output vector: " + std::to_string(count));
    }
    CombineVectorNode(const DataLine & input1, const DataLine & input2) : CombineVectorNode(MakeVector(input1, input2)) { }
    CombineVectorNode(const DataLine & input1, const DataLine & input2, const DataLine & input3) : CombineVectorNode(MakeVector(input1, input2, input3)) { }
    CombineVectorNode(const DataLine & input1, const DataLine & input2, const DataLine & input3, const DataLine & input4) : CombineVectorNode(MakeVector(input1, input2, input3, input4)) { }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    unsigned int count;

    static unsigned int CountElements(const std::vector<DataLine> & inputs)
    {
        unsigned int count = 0;
        for (int i = 0; i < inputs.size(); ++i)
            count += inputs[i].GetDataLineSize();
        return count;
    }
};