#include "DistanceNode.h"

unsigned int DistanceNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return 1;
}
std::string DistanceNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_distance";
}

DistanceNode::DistanceNode(const DataLine & pos1, const DataLine & pos2, std::string name)
    : DataNode(MakeVector(pos1, pos2),
               [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new DistanceNode(ins[0], ins[1], _name)); },
               name)
{
    Assert(pos1.GetSize() == pos2.GetSize(), "The two position inputs aren't the same size!");
}

void DistanceNode::WriteMyOutputs(std::string & outCode) const
{
    std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
    outCode += "\t" + vecType + " " + GetOutputName(0) + " = distance(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
}