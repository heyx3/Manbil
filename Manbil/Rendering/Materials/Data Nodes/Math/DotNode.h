#pragma once

#include "../DataNode.h"


//Outputs the dot product of two input nodes.
class DotNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "dotProductNode"; }

    DotNode(const DataLine & first, const DataLine & second)
        : DataNode(MakeVector(first, second), MakeVector(0))
    {
        assert(first.GetDataLineSize() == second.GetDataLineSize());
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        std::string vecType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) +
                        " = dot(" + GetInputs()[0].GetValue(shaderType) + ", " +
                                    GetInputs()[1].GetValue(shaderType) + ");\n";
    }
};