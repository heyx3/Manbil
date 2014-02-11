#pragma once

#include "../DataNode.h"


//A node that rounds the input to an integer away from 0.
class CeilNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { "ceilNode"; }

    CeilNode(const DataLine & input) : DataNode(MakeVector(input), MakeVector(input.GetDataLineSize())) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) + " = ceil(" + GetInputs()[0].GetValue(shaderType) + ");\n";
    }
};