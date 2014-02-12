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

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        outCode += "\t" + Vector(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0) + " = reflect(" + GetInputs()[0].GetValue() + ", ";
        
        if (isNormNormalized) outCode += GetInputs()[1].GetValue();
        else outCode += "normalize(" + GetInputs()[1].GetValue() + ")";

        outCode += ");\n";
    }


private:

    bool isNormNormalized;
};