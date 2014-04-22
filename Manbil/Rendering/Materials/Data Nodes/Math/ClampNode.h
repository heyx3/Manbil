#pragma once

#include "../DataNode.h"


//Keeps a value inside a max and a min.
class ClampNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "clampNode"; }

    ClampNode(const DataLine & min, const DataLine & max, const DataLine & value)
        : DataNode(MakeVector(min, max, value), MakeVector(min.GetDataLineSize()))
    {
        Assert(min.GetDataLineSize() == max.GetDataLineSize(),
               "Min and max value aren't the same size!");
        Assert(min.GetDataLineSize() == value.GetDataLineSize(),
               "Min/max and value to clamp aren't the same size!");
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = VectorF(GetOutputs()[0]).GetGLSLType();
        outCode += "\t" + vecType + " " + GetOutputName(0) +
                    " = clamp(" + GetValueInput().GetValue() + ", " +
                                  GetMinInput().GetValue() + ", " +
                                  GetMaxInput().GetValue() + ");\n";
    }


private:

    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetValueInput(void) const { return GetInputs()[2]; }
};