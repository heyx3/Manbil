#pragma once

#include "../DataNode.h"


//Node that gets the absolute value of an input.
class AbsNode : public DataNode
{
public:

    AbsNode(DataLine input)
        : DataNode(MakeVector(input), MakeVector(input.GetDataLineSize()))
    {

    }

    virtual std::string GetName(void) const override { return "absNode"; }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();

        outCode += "\t" + vecType + GetOutputName(0) + " = abs(" + GetInputs()[0].GetValue() + ");\n";
    }
};