#pragma once

#include "../DataNode.h"


//Outputs the dot product of two input nodes.
class DotNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "dotProduct"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetInputs()[0].GetSize();
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetName() + "_dotted";
    }

    DotNode(const DataLine & first, const DataLine & second, std::string name = "")
        : DataNode(MakeVector(first, second),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new DotNode(ins[0], ins[1], _name)); },
                   name)
    {
        Assert(first.GetSize() == second.GetSize(), "The two inputs must be the same size!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = VectorF(GetInputs()[0].GetSize()).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0) +
                        " = dot(" + GetInputs()[0].GetValue() + ", " +
                                    GetInputs()[1].GetValue() + ");\n";
    }
};