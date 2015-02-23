#include "VectorComponentsNode.h"


ADD_NODE_REFLECTION_DATA_CPP(VectorComponentsNode, 1.0f)


#pragma warning(disable: 4100)
unsigned int VectorComponentsNode::GetOutputSize(unsigned int index) const
{
    return 1;
}
#pragma warning(default: 4100)

std::string VectorComponentsNode::GetOutputName(unsigned int index) const
{
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
    : DataNode(MakeVector(inData), name)
{

}

#pragma warning(disable: 4100)
std::string VectorComponentsNode::GetInputDescription(unsigned int index) const
{
    return "toSplit";
}
#pragma warning(default: 4100)

#pragma warning(disable: 4100)
void VectorComponentsNode::WriteMyOutputs(std::string & outCode) const
{
    //No need to write anything; we're just pulling out components of another vector.
}
#pragma warning(default: 4100)