#pragma once

#include "../DataNode.h"

//TODO: Pull out into .cpp.

//Outputs the lerp component for a given value when given a max and min.
class GetLerpComponentNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "getLerpComponentNode"; }
    virtual std::string GetOutputName(unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "output index must be 0, not " + std::to_string(outputIndex));
        return GetName() + std::to_string(GetUniqueID()) + "_lerpComponent";
    }

    GetLerpComponentNode(const DataLine & min, const DataLine & max, const DataLine & value)
        : DataNode(MakeVector(min, max, value), MakeVector(min.GetDataLineSize()))
    {
        Assert(min.GetDataLineSize() == max.GetDataLineSize(),
               std::string() + "'min' size is " + std::to_string(min.GetDataLineSize()) + " but 'max' size is " + std::to_string(max.GetDataLineSize()));
        Assert(value.GetDataLineSize() == min.GetDataLineSize() || value.GetDataLineSize() == 1,
               std::string() + "Value must be size 1 or " + std::to_string(min.GetDataLineSize()) + " but is it size " + std::to_string(value.GetDataLineSize()));
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string min = GetMinInput().GetValue(),
                    max = GetMaxInput().GetValue(),
                    value = GetValueInput().GetValue();
        outCode += "\t" + VectorF(GetMinInput().GetDataLineSize()).GetGLSLType() + " " + GetOutputName(0) + " = " +
            "(" + value + " - " + min + ") / (" + max + " - " + min + ");\n";
    }

private:

    const DataLine & GetMinInput(void) const { return GetInputs()[0]; }
    const DataLine & GetMaxInput(void) const { return GetInputs()[1]; }
    const DataLine & GetValueInput(void) const { return GetInputs()[2]; }
};