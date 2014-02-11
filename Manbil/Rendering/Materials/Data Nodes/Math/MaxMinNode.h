#pragma once

#include "../DataNode.h"


//Outputs the max or min of another Node.
class MaxMinNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return (isMax ? "maxNode" : "minNode"); }

    //Takes whether this node is a max or a min.
    MaxMinNode(const DataLine & input, bool _isMax) : DataNode(MakeVector(input), MakeVector(input.GetDataLineSize())), isMax(_isMax) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType)
    {
        std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) + " = " + GetGLSLFunc() + "(" + GetInputs()[0].GetValue(shaderType) + ");\n";
    }


private:

    //If "false", this is min.
    bool isMax;

    std::string GetGLSLFunc(void) const { return (isMax ? "max" : "min"); }
};