#pragma once

#include "../DataNode.h"


//A node that rounds the input to an integer towards 0.
class FloorNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { "floorNode"; }

    FloorNode(const DataLine & input) : DataNode(MakeVector(input), MakeVector(input.GetDataLineSize())) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) + " = floor(" + GetInputs()[0].GetValue(shaderType) + ");\n";
    }
};