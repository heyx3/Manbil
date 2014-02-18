#pragma once

#include "../DataNode.h"


//Raises an input to the power of another input.
class PowNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "powNode"; }

    const DataLine & GetBaseInput(void) const { return GetInputs()[0]; }
    const DataLine & GetExponentInput(void) const { return GetInputs()[1]; }

    PowNode(const DataLine & base, const DataLine & exponent)
        : DataNode(MakeVector(base, exponent), MakeVector(base.GetDataLineSize()))
    {
        assert(exponent.GetDataLineSize() == base.GetDataLineSize());
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        outCode += "\t" + Vector(GetBaseInput().GetDataLineSize()).GetGLSLType() + " " + GetOutputName(0) +
                                    " = pow(" + GetBaseInput().GetValue() + ", " +
                                                GetExponentInput().GetValue() + ");\n";
    }
};