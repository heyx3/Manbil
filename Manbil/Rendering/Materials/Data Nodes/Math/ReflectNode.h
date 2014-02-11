#pragma once

#include "../DataNode.h"


//Outputs a vector reflected around another vector.
class ReflectNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "reflectNode"; }

    ReflectNode(const DataLine & toReflect, const DataLine & reflectNormal, bool isNormalNormalized)
        : DataNode(MakeVector(toReflect, reflectNormal), MakeVector(toReflect.GetDataLineSize())), isNormNormalized(isNormalNormalized)
    {
        assert(toReflect.GetDataLineSize() == reflectNormal.GetDataLineSize());
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        outCode += "\t" + Vector(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0, shaderType) + " = reflect(" + GetInputs()[0].GetValue(shaderType) + ", ";
        
        if (isNormNormalized) outCode += GetInputs()[1].GetValue(shaderType);
        else outCode += "normalize(" + GetInputs()[1].GetValue(shaderType) + ")";

        outCode += ");\n";
    }


private:

    bool isNormNormalized;
};