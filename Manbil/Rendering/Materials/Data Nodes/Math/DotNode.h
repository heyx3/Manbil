#pragma once

#include "../DataNode.h"


//Outputs the dot product of two input nodes.
class DotNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "dotProductNode"; }

    DotNode(const DataLine & first, const DataLine & second)
        : DataNode(MakeVector(first, second), MakeVector(first.GetDataLineSize()))
    {
        Assert(first.GetDataLineSize() == second.GetDataLineSize(),
               "The two inputs must be the same size!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = VectorF(GetInputs()[0].GetDataLineSize()).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0) +
                        " = dot(" + GetInputs()[0].GetValue() + ", " +
                                    GetInputs()[1].GetValue() + ");\n";
    }
};