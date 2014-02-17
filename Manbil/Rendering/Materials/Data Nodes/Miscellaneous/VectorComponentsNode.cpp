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
    switch (outputIndex)
    {
    case 0: ret += "x";
    case 1: ret += "y";
    case 2: ret += "z";
    case 3: ret += "w";
    default: assert(false);
    }

    return "";
}

void VectorComponentsNode::WriteMyOutputs(std::string & outCode) const
{
    for (int i = 0; i < GetVectorInput().GetDataLineSize(); ++i)
    {
        outCode += "\tfloat " + GetOutputName(i) + " = " + GetVectorInput().GetValue() + ".";
        switch (i)
        {
        case 0: outCode += "x";
        case 1: outCode += "y";
        case 2: outCode += "z";
        case 3: outCode += "w";
        default: assert(false);
        }
        outCode += "\n";
    }
}