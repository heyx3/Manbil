#pragma once

#include "../DataNode.h"


//A node that outputs the cross product of two vec3's.
class CrossNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "crossProductNode"; }
    
    CrossNode(const DataLine & firstL, const DataLine & secondL)
        : DataNode(MakeVector(firstL, secondL), MakeVector(3))
    {
        Assert(GetInputs()[0].GetDataLineSize() == 3, "First input isn't a vec3!");
        Assert(GetInputs()[1].GetDataLineSize() == 3, "Second input isn't a vec3!");
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        outCode += "\tvec3 " + GetOutputName(0) + " = cross(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
    }
};