#include "RefractNode.h"

unsigned int RefractNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return 3;
}
std::string RefractNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    GetName() + "_refracted";
}

RefractNode::RefractNode(const DataLine & toRefract, const DataLine & refractNormal, const DataLine & indexOfRefraction, std::string name)
    : DataNode(MakeVector(toRefract, refractNormal, indexOfRefraction),
               [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new RefractNode(ins[0], ins[1], ins[2], _name)); },
               name)
{
    Assert(toRefract.GetSize() == 3, "'toRefract' doesn't have a size of 3!");
    Assert(refractNormal.GetSize() == 3, "'refractNormal' doesn't have a size of 3!");
    Assert(indexOfRefraction.GetSize() == 1, "'indexOfRefraction' isn't a float!");
}

void RefractNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\t" + VectorF(GetOutputSize(0)).GetGLSLType() + " " + GetOutputName(0) +
        " = reflect(" + GetInputs()[0].GetValue() + ", " +
        GetInputs()[1].GetValue() + ", " +
        GetInputs()[2].GetValue() + ");\n";
}

std::string RefractNode::GetInputDescription(unsigned int index) const
{
    return (index == 0 ? "Vector to refract" : (index == 1 ? "Refraction normal vector" : "Index of refraction"));
}