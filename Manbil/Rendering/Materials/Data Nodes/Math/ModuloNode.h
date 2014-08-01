#pragma once

#include "../DataNode.h"


//Outputs an input mod another input.
class ModuloNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "modulo"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetInputs()[0].GetSize();
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetName() + "_result";
    }

    ModuloNode(const DataLine & numerator, const DataLine & divisor, std::string name = "")
        : DataNode(MakeVector(numerator, divisor),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new ModuloNode(ins[0], ins[1], _name)); },
                   name)
    {
        Assert(numerator.GetSize() == divisor.GetSize() || divisor.GetSize() == 1,
               "Divisor must be size 1 or " + ToString(numerator.GetSize()) +
                   ", but it is size " + ToString(divisor.GetSize()) + "!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = VectorF(GetInputs()[0].GetSize()).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0) +
            " = mod(" + GetInputs()[0].GetValue() + ", " +
                        GetInputs()[1].GetValue() + ");\n";
    }

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        return (index == 0) ? "Numerator" : "Denominator";
    }
};