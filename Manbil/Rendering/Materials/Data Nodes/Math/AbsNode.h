#pragma once

#include "../DataNode.h"


//Node that gets the absolute value of an input.
class AbsNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "absValueNode"; }

    AbsNode(DataLine input)
        : DataNode(MakeVector(input), MakeVector(input.GetDataLineSize()))
    {

    }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        std::string vecType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();

        outCode += "\t" + vecType + GetOutputName(0, shaderType) + " = abs(" + GetInputs()[0].GetValue(shaderType) + ");\n";
    }
};