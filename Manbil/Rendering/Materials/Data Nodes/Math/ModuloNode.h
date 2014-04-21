#pragma once

#include "../DataNode.h"


//Outputs an input mod another input.
class ModuloNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "moduloNode"; }

    ModuloNode(const DataLine & numerator, const DataLine & divisor)
        : DataNode(MakeVector(numerator, divisor), MakeVector(numerator.GetDataLineSize()))
    {
        Assert(numerator.GetDataLineSize() == divisor.GetDataLineSize() ||
               divisor.GetDataLineSize() == 1,
               "Divisor must be size 1 or " + std::to_string(numerator.GetDataLineSize()) +
               ", but it is size " + std::to_string(divisor.GetDataLineSize()) + "!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = VectorF(GetInputs()[0].GetDataLineSize()).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0) +
            " = mod(" + GetInputs()[0].GetValue() + ", " +
                        GetInputs()[1].GetValue() + ");\n";
    }
};