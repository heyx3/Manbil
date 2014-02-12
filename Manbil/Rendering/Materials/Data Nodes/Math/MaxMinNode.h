#pragma once

#include "../DataNode.h"


//Outputs the max or min of another Node.
class MaxMinNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return (isMax ? "maxNode" : "minNode"); }

    //Takes whether this node is a max or a min.
    MaxMinNode(const DataLine & input1, const DataLine & input2, bool _isMax)
        : DataNode(MakeVector(input1, input2), MakeVector(BasicMath::Max(input1.GetDataLineSize(), input2.GetDataLineSize()))), isMax(_isMax)
    {
        assert(input1.GetDataLineSize() == 1 || input2.GetDataLineSize() == 1 ||
               input1.GetDataLineSize() == input2.GetDataLineSize());
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = Vector(GetOutputs()[0]).GetGLSLType();

        outCode += "\t" + vecType + " " + GetOutputName(0) + " = " + GetGLSLFunc() + "(" + GetInputs()[0].GetValue() + ");\n";
    }


private:

    //If "false", this is min.
    bool isMax;

    std::string GetGLSLFunc(void) const { return (isMax ? "max" : "min"); }
};