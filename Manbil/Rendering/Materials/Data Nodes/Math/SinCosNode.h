#pragma once

#include "../DataNode.h"


//Outputs the sine or cosine of another Node.
class SinCosNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return (isSine ? "sinNode" : "cosNode"); }

    //Takes whether this node is a sine function or a cosine function.
    SinCosNode(const DataLine & input, bool _isSine) : DataNode(MakeVector(input), MakeVector(input.GetDataLineSize())), isSine(_isSine) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType)
    {
        std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) + " = " + GetGLSLFunc() + "(" + GetInputs()[0].GetValue(shaderType) + ");\n";
    }


private:

    //If "false", this is cosine.
    bool isSine;

    std::string GetGLSLFunc(void) const { return (isSine ? "sin" : "cos"); }
};