#include "CombineVectorNode.h"


MAKE_NODE_READABLE_CPP(CombineVectorNode, 1.0f, 1.0f)


unsigned int CombineVectorNode::GetOutputSize(unsigned int index) const
{
    unsigned int count = 0;
    for (unsigned int i = 0; i < GetInputs().size(); ++i)
        count += GetInputs()[i].GetSize();
    return count;
}
std::string CombineVectorNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_combined";
}


CombineVectorNode::CombineVectorNode(const std::vector<DataLine> & inputs, std::string name)
    : DataNode(inputs, name)
{
}


void CombineVectorNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs().size() > 0, "There aren't any elements in the output vector!");
    Assert(GetInputs().size() < 5, "Too many elements in the output vector! Can only have up to 4, but there are " + ToString(GetInputs().size()));
}


void CombineVectorNode::WriteMyOutputs(std::string & outCode) const
{
    unsigned int outSize = GetOutputSize(0);

    std::string vecType = VectorF(outSize).GetGLSLType();

    outCode += "\t" + vecType + " " + GetOutputName(0) + " = " + vecType + "(";

    unsigned int counter = 1;
    for (unsigned int input = 0; input < GetInputs().size(); ++input)
    {
        const DataLine & inp = GetInputs()[input];

        for (unsigned int element = 0; element < inp.GetSize(); ++element)
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