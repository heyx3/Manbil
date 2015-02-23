#include "RotateAroundAxisNode.h"

#include "../../../../Math/Lower Math/Quaternion.h"


ADD_NODE_REFLECTION_DATA_CPP(RotateAroundAxisNode, Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), 0.0f)

RotateAroundAxisNode::RotateAroundAxisNode(const DataLine & toRot, const DataLine & axis, const DataLine & angle, std::string name)
    : DataNode(MakeVector(toRot, axis, angle), name)
{
}

void RotateAroundAxisNode::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
    std::string rotFunc = "vec3 " + GetName() + "_rotFunc(vec3 toRot, vec3 rotAxis, float rotAmount) \n\
{                                                                                                    \n\
    float halfAng = rotAmount * 0.5f;                                                                \n\
    vec3 quatXYZ = sin(halfAng) * rotAxis;                                                           \n\
    return toRot + (2.0f * cross(quatXYZ, cross(quatXYZ, toRot) + (toRot * cos(halfAng))));          \n\
}\n";
    outDecls.insert(outDecls.end(), rotFunc);
}

void RotateAroundAxisNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec3 " + GetOutputName(0) + " = " +
                            GetName() + "_rotFunc(" + GetToRotateInput().GetValue() + ", " +
                                                  GetRotateAxisInput().GetValue() + ", " +
                                                  GetRotateAmountInput().GetValue() + ");\n";
}

std::string RotateAroundAxisNode::GetInputDescription(unsigned int index) const
{
    switch (index)
    {
        case 0: return "Vector3 to rotate";
        case 1: return "Axis to rotate around";
        case 2: return "Angle to rotate by";

        default:
            Assert(false, "Unexpected input index " + ToString(index));
            return "INVALID_INPUT_INDEX_" + ToString(index);
    }
}

void RotateAroundAxisNode::AssertMyInputsValid(void) const
{
    Assert(GetToRotateInput().GetSize() == 3,
           "Value to rotate should be size 3, but it is size " + ToString(GetToRotateInput().GetSize()));
    Assert(GetRotateAxisInput().GetSize() == 3,
           "Axis to rotate around should be size 3, but it is size " + ToString(GetRotateAxisInput().GetSize()));
    Assert(GetRotateAmountInput().GetSize() == 1,
           "Angle to rotate by should be size 1, but it is size " + ToString(GetRotateAmountInput().GetSize()));
}