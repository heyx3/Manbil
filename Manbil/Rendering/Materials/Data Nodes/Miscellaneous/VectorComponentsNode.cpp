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
    Assert(outputIndex < GetVectorInput().GetDataLineSize(),
           std::string() + "Invalid output index " + std::to_string(outputIndex) + "!");
    
    std::string ret = GetInputs()[0].GetValue() + ".";

    //Swizzling is only valid if the input isn't a float.
    if (GetVectorInput().GetDataLineSize() > 1)
        switch (outputIndex)
        {
            case 0: ret += "x"; break;
            case 1: ret += "y"; break;
            case 2: ret += "z"; break;
            case 3: ret += "w"; break;
            default: Assert(false, std::string() + "Invalid output index " + std::to_string(outputIndex) + "!");
        }

    return ret;
}