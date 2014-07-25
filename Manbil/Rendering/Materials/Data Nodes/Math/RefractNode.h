#pragma once

#include "../DataNode.h"


//Outputs a vector refracted around another vector.
class RefractNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "reflectNode"; }

    RefractNode(const DataLine & toRefract, const DataLine & refractNormal, const DataLine & indexOfRefraction)
        : DataNode(MakeVector(toRefract, refractNormal, indexOfRefraction), MakeVector(toRefract.GetDataLineSize()))
    {
        Assert(toRefract.GetDataLineSize() == refractNormal.GetDataLineSize(),
               "'toRefract' doesn't have the same size as 'refractNormal'!");
        Assert(indexOfRefraction.GetDataLineSize() == 1, "'indexOfRefraction' isn't a float!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        outCode += "\t" + VectorF(GetOutputs()[0]).GetGLSLType() + " " + GetOutputName(0) +
                        " = reflect(" + GetInputs()[0].GetValue() + ", " +
                                    GetInputs()[1].GetValue() + ", " +
                                    GetInputs()[2].GetValue() + ");\n";
    }
};