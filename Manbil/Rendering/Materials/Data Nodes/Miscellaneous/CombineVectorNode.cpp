#include "CombineVectorNode.h"

void CombineVectorNode::WriteMyOutputs(std::string & outCode, Shaders shaderType) const
{
    std::string vecType = Vector(count).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0, shaderType) + " = " + vecType + "(";

    unsigned int counter = 1;
    for (int input = 0; input < GetInputs().size(); ++input)
    {
        const DataLine & inp = GetInputs()[input];

        for (int element = 0; element < inp.GetDataLineSize(); ++element)
        {
            outCode += inp.GetValue(shaderType) + ".";

            switch (counter)
            {
            case 1: outCode += "x";
            case 2: outCode += "y";
            case 3: outCode += "z";
            case 4: outCode += "w";
            default: assert(false);
            }

            if (counter < count) outCode += ", ";

            counter += 1;
        }
    }

    outCode += ");\n";
}