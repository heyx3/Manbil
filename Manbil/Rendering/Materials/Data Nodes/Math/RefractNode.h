#pragma once

#include "../DataNode.h"


//Outputs a vector refracted around another vector.
class RefractNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "reflectNode"; }

    RefractNode(const DataLine & toRefract, const DataLine & refractNormal, const DataLine & indexOfRefraction, bool isNormalNormalized)
        : DataNode(MakeVector(toRefract, refractNormal, indexOfRefraction), MakeVector(toRefract.GetDataLineSize())),
                   isNormNormalized(isNormalNormalized)
    {
        assert(toRefract.GetDataLineSize() == refractNormal.GetDataLineSize() &&
               indexOfRefraction.GetDataLineSize() == 1);
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        outCode += "\t" + VectorF(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0) + " = reflect(" + GetInputs()[0].GetValue() + ", ";

        if (isNormNormalized) outCode += GetInputs()[1].GetValue();
        else outCode += "normalize(" + GetInputs()[1].GetValue() + ")";

        outCode += ", " + GetInputs()[2].GetValue() + ");\n";
    }


private:

    bool isNormNormalized;
};