#include "VectorComponentsNode.h"


unsigned int VectorComponentsNode::GetOutputSize(unsigned int index) const
{
    Assert(index < GetNumbOutputs(), "Invalid output index " + ToString(index));
    return 1;
}
std::string VectorComponentsNode::GetOutputName(unsigned int index) const
{
    Assert(index < GetNumbOutputs(), "Invalid output index " + ToString(index));

    std::string ret = GetInputs()[0].GetValue() + ".";

    //Swizzling is only valid if the input isn't a float.
    if (GetInputs()[0].GetSize() > 1)
    {
        switch (index)
        {
            case 0: ret += "x"; break;
            case 1: ret += "y"; break;
            case 2: ret += "z"; break;
            case 3: ret += "w"; break;
            default: Assert(false, std::string() + "Sanity check fail: unexpected output index " + ToString(index) + "!");
        }
    }

    return ret;
}

VectorComponentsNode::VectorComponentsNode(const DataLine & inData, std::string name)
    : DataNode(MakeVector(inData),
               []() { return DataNodePtr(new VectorComponentsNode(DataLine(1.0f))); },
               name)
{

}

std::string VectorComponentsNode::GetInputDescription(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return "toSplit";
}

#pragma warning(disable: 4100)
void VectorComponentsNode::WriteMyOutputs(std::string & outCode) const
{
    //No need to write anything; we're just pulling out components of another vector.
}
#pragma warning(default: 4100)