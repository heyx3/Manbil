#pragma once

#include "../DataNode.h"


//Gets the normalized version of the input vector.
class NormalizeNode : public DataNode
{
public:

    virtual std::string GetName(void) const { return "normalizeNode"; }

    NormalizeNode(const DataLine & pos)
        : DataNode(MakeVector(pos), MakeVector(pos.GetDataLineSize()))
    {

    }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();
        outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) + " = distance(" + GetInputs()[0].GetValue(shaderType) + ", " + GetInputs()[1].GetValue(shaderType) + ");\n";
    }
};