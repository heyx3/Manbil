#include "CombineVectorNode.h"



unsigned int CombineVectorNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    unsigned int count = 0;
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
        count += GetInputs()[i].GetSize();
    return count;
}


CombineVectorNode::CombineVectorNode(const std::vector<DataLine> & inputs, std::string name)
    : DataNode(inputs,
               [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new CombineVectorNode(ins, _name)); },
               name)
{
    Assert(inputs.size() > 0, "There aren't any elements in the output vector!");
    Assert(inputs.size() < 5, "Too many elements in the output vector! Can only have up to 4, but there are " + ToString(inputs.size()));
}


void CombineVectorNode::WriteMyOutputs(std::string & outCode) const
{
    unsigned int outSize = GetOutputSize(0);

    std::string vecType = VectorF(outSize).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = " + vecType + "(";

    unsigned int counter = 1;
    for (int input = 0; input < GetInputs().size(); ++input)
    {
        const DataLine & inp = GetInputs()[input];

        for (int element = 0; element < inp.GetSize(); ++element)
        {
            outCode += inp.GetValue();
            if (inp.GetSize() > 1)
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

            if (counter < outSize) outCode += ", ";

            counter += 1;
        }
    }

    outCode += ");\n";
}