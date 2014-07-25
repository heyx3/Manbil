#pragma once

#include "../DataNode.h"

//TODO: Pull into .cpp.

//Outputs a vector reflected around another vector.
class ReflectNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "reflectNode"; }

    ReflectNode(const DataLine & toReflect, const DataLine & reflectNormal)
        : DataNode(MakeVector(toReflect, reflectNormal), MakeVector(toReflect.GetDataLineSize()))
    {
        Assert(toReflect.GetDataLineSize() == reflectNormal.GetDataLineSize(),
               "'toReflect' isn't the same size as 'reflectNormal'!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        outCode += "\t" + VectorF(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0) + " = reflect(" + GetInputs()[0].GetValue() + ", ";
        
        outCode += GetInputs()[1].GetValue();

        outCode += ");\n";
    }
};