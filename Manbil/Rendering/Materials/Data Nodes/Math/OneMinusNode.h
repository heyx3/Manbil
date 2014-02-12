#pragma once

#include "../DataNode.h"


//Outputs 1.0 minus the input node.
class OneMinusNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "oneMinusNode"; }

    OneMinusNode(const DataLine & input) : DataNode(MakeVector(input), MakeVector(input.GetDataLineSize())) { }

    virtual std::string GetOutputName(unsigned int index, Shaders shaderType) const override { assert(index == 0); return GetName() + std::to_string(GetUniqueID()) + "_" + "invValue"; }

protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        std::string vecType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) + " = 1.0 - " + GetInputs()[0].GetValue(shaderType) + ";\n";
    }
};