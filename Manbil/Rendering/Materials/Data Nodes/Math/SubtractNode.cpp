#include "SubtractNode.h"


void SubtractNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = Vector(GetInputs()[0].GetDataLineSize()).GetGLSLType();

    outCode += "\t" + vecType + GetOutputName(0) + " = ";
    for (int i = 0; i < GetInputs().size(); ++i)
    {
        outCode += GetInputs()[i].GetValue();

        if (i < GetInputs().size() - 1) outCode += " - ";
        else outCode += ";\n";
    }
}