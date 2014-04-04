#include "VectorComponentsNode.h"

std::vector<unsigned int> VectorComponentsNode::buildOutputs(const DataLine & inData)
{
    std::vector<unsigned int> ret;
    for (unsigned int i = 0; i < inData.GetDataLineSize(); ++i)
    {
        ret.insert(ret.end(), 1);
    }
    return ret;
}

std::string VectorComponentsNode::GetOutputName(unsigned int outputIndex) const
{
    assert(outputIndex < GetVectorInput().GetDataLineSize());
    
    std::string ret = GetName() + std::to_string(GetUniqueID()) + "_";

    //std::string ret = GetVectorInput().GetValue();
    //Swizzling is only valid if the input isn't a float.
    if (GetVectorInput().GetDataLineSize() > 1)
        switch (outputIndex)
        {
            case 0: ret += "x"; break;
            case 1: ret += "y"; break;
            case 2: ret += "z"; break;
            case 3: ret += "w"; break;
            default: assert(false);
        }

    return ret;
}

void VectorComponentsNode::WriteMyOutputs(std::string & outCode) const
{
    for (unsigned int i = 0; i < GetVectorInput().GetDataLineSize(); ++i)
    {
        outCode += "\tfloat " + GetOutputName(i) + " = " + GetVectorInput().GetValue();

        if (GetVectorInput().GetDataLineSize() > 1)
        {
            switch (i)
            {
                case 0: outCode += ".x"; break;
                case 1: outCode += ".y"; break;
                case 2: outCode += ".z"; break;
                case 3: outCode += ".w"; break;
                default: assert(false);
            }
        }
        outCode += ";\n";

    }
}