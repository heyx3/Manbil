#include "RefractNode.h"


MAKE_NODE_READABLE_CPP(RefractNode, Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), 1.0f)


unsigned int RefractNode::GetOutputSize(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return 3;
}
std::string RefractNode::GetOutputName(unsigned int index) const
{
    Assert(index == 0, "Invalid output index " + ToString(index));
    return GetName() + "_refracted";
}

RefractNode::RefractNode(const DataLine & toRefract, const DataLine & refractNormal, const DataLine & indexOfRefraction, std::string name)
    : DataNode(MakeVector(toRefract, refractNormal, indexOfRefraction), name)
{

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

void RefractNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 3, "'toReflect' input isn't size 3!");
    Assert(GetInputs()[1].GetSize() == 3, "'reflectNormal' input isn't size 3!");
    Assert(GetInputs()[2].GetSize() == 1, "'indexOfRefraction' isn't size 1!");
}