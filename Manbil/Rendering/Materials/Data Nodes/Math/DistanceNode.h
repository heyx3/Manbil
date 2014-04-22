#pragma once

#include "../DataNode.h"


//Outputs the distance between two vectors.
class DistanceNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "distanceNode"; }

    DistanceNode(const DataLine & pos1, const DataLine & pos2)
        : DataNode(MakeVector(pos1, pos2), MakeVector(1))
    {
        Assert(pos1.GetDataLineSize() == pos2.GetDataLineSize(),
               "The two position inputs aren't the same size!");
    }


protected:
    
    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = VectorF(GetOutputs()[0]).GetGLSLType();
        outCode += "\t" + vecType + " " + GetOutputName(0) + " = distance(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
    }
};