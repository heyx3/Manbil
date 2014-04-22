#include "CombineVectorNode.h"

void CombineVectorNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(count).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = " + vecType + "(";

    unsigned int counter = 1;
    for (int input = 0; input < GetInputs().size(); ++input)
    {
        const DataLine & inp = GetInputs()[input];

        for (int element = 0; element < inp.GetDataLineSize(); ++element)
        {
            outCode += inp.GetValue();
            if (inp.GetDataLineSize() > 1)
            {
                switch (counter)
                {
                    case 1: outCode += ".x"; break;
                    case 2: outCode += ".y"; break;
                    case 3: outCode += ".z"; break;
                    case 4: outCode += ".w"; break;
                    default: Assert(false, std::string() + "Invalid index: " + std::to_string(counter));
                }
            }

            if (counter < count) outCode += ", ";

            counter += 1;
        }
    }

    outCode += ");\n";
}