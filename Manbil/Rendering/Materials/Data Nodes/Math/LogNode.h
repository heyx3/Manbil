#pragma once

#include "../DataNode.h"


//Outputs the logarithm of an input node with a different input node's base.
class LogNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "log"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetInputs()[0].GetSize();
    }
    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetInputs()[0].GetSize();
    }

    LogNode(const DataLine & value, DataLine base = DataLine(VectorF(2.0f)), std::string name = "")
        : DataNode(MakeVector(value, base),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new LogNode(ins[0], ins[1], _name)); },
                   name)
    {
        Assert(base.GetSize() == 1, "Base must have a size of 1! It has size " + ToString(base.GetSize()));
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        return (index == 0 ? "Value" : "Log Base");
    }
};