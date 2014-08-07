#include "RotateAroundAxisNode.h"

#include "../../../../Math/Quaternion.h"


MAKE_NODE_READABLE_CPP(RotateAroundAxisNode, Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), 0.0f)

RotateAroundAxisNode::RotateAroundAxisNode(const DataLine & toRot, const DataLine & axis, const DataLine & angle, std::string name)
    : DataNode(MakeVector(toRot, axis, angle), name)
{
}

void RotateAroundAxisNode::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
    //Old way of computing rotation:
    //   return toRot + (2.0 * cross(cross(vector, quaternion.xyz) +
    //                                  (quaternion.w * toRot),
    //                               quaternion.xyz));
    //NOTE: I think the "vector" term up there is a mistake and should have been replaced with "toRot".
    //New way of computing rotation:
    //   vec3 temp = 2.0f * cross(quaternion.xyz, toRot);
    //   return toRot + (quaternion.w * temp) + cross(quaternion.xyz, temp);
    //TODO: Test this node's permutations.


    bool axisConst = GetRotateAxisInput().IsConstant(),
         angleConst = GetRotateAmountInput().IsConstant(),
         inputConst = GetToRotateInput().IsConstant();

    Vector3f axis, input;
    float angle;

    if (axisConst) axis = *(Vector3f*)GetRotateAxisInput().GetConstantValue().GetValue();
    if (inputConst) input = *(Vector3f*)GetToRotateInput().GetConstantValue().GetValue();
    if (angleConst) angle = GetRotateAmountInput().GetConstantValue().GetValue()[0];


    std::string rotFunc = "vec3 " + GetName() + "_rotFunc(";

#pragma warning(disable: 4701)
    if (axisConst && angleConst && inputConst)
    {
        Quaternion rot(axis, angle);
        Vector3f rotated = rot.Rotated(input);

        rotFunc += ") { return vec3(" + std::to_string(rotated.x) + ", " + std::to_string(rotated.y) + ", " + std::to_string(rotated.z) + "); }\n\n";
    }
    else if (angleConst && axisConst)
    {
        Quaternion rot(axis, angle);
        rotFunc += "vec3 toRot)\n\
{\n\
    vec3 quatXYZ = vec3(" + std::to_string(rot.x) + ", " + std::to_string(rot.y) + ", " + std::to_string(rot.z) + ");\n\
    vec3 temp = 2.0f * cross(quatXYZ, toRot);\n\
    return toRot + (" + std::to_string(rot.w) + " * temp) + cross(quatXYZ, temp);\n\
}\n\n";
    }
    else if (angleConst && inputConst)
    {
        float angleSin = sinf(angle * 0.5f),
              angleCos = cosf(angle * 0.5f);
        rotFunc += "vec3 rotAxis)\n\
{\n\
    vec4 quatXYZ = rotAxis.xyz * " + std::to_string(angleSin) + ";\n\
    vec3 temp = 2.0f * cross(quatXYZ, " + GetToRotateInput().GetValue() + ");\n\
    return " + GetToRotateInput().GetValue() + " + (" + std::to_string(angleCos) + " * temp) + cross(quatXYZ, temp);\n\
}\n\n";
    }
    else if (inputConst && axisConst)
    {
        rotFunc += "float rotAngle)\n\
{\n\
    vec4 quatXYZ = vec3(" + std::to_string(axis.x) + ", " + std::to_string(axis.y) + ", " + std::to_string(axis.z) + ") * \n\
                        sin(rotAngle * 0.5f));\n\
    vec3 temp = 2.0f * cross(quatXYZ, " + GetToRotateInput().GetValue() + ");\n\
    return " + GetToRotateInput().GetValue() + " + (cos(rotAngle * 0.5f) * temp) + cross(quatXYZ, temp);\n\
}\n\n";
    }
    else if (angleConst)
    {
        float angleSin = sinf(angle * 0.5f),
              angleCos = cosf(angle * 0.5f);
        rotFunc += "vec3 toRot, vec3 rotAxis)\n\
{\n\
    vec3 quatXYZ = vec3(rotAxis * " + std::to_string(angleSin) + ");\n\
    vec3 temp = 2.0f * cross(quatXYZ, toRot);\n\
    return toRot + (" + std::to_string(angleCos) + " * temp) + cross(quatXYZ, temp);\n\
}\n\n";
    }
    else if (axisConst)
    {
        rotFunc += "vec3 toRot, float rotAngle)\n\
{\n\
    vec3 quatXYZ = vec3(" + GetRotateAxisInput().GetValue() + " * sin(rotAngle * 0.5f));\n\
    vec3 temp = 2.0f * cross(quatXYZ, toRot);\n\
    return toRot + (cos(rotAngle * 0.5f) * temp) + cross(quatXYZ, temp);\n\
}\n\n";
    }
    else if (inputConst)
    {
        rotFunc += "vec3 rotAxis, vec3 rotAngle)\n\
{\n\
    vec3 quatXYZ = vec3(rotAxis * sin(rotAngle * 0.5f));\n\
    vec3 temp = 2.0f * cross(quatXYZ, " + GetToRotateInput().GetValue() + ");\n\
    return " + GetToRotateInput().GetValue() + " + (cos(rotAngle * 0.5f) * temp) + cross(quatXYZ, temp);\n\
}\n\n";
    }
    else
    {
        rotFunc += "vec3 toRot, vec3 rotAxis, float rotAngle)\n\
{\n\
    vec3 quatXYZ = vec3(rotAxis * sin(rotAngle * 0.5f));\n\
    vec3 temp = 2.0f * cross(quatXYZ, toRot);\n\
    return toRot + (cos(rotAngle * 0.5f) * temp) + cross(quatXYZ, temp);\n\
}\n\n";
    }
#pragma warning(default: 4701)

    outDecls.insert(outDecls.end(), rotFunc);
}

void RotateAroundAxisNode::WriteMyOutputs(std::string & outCode) const
{
    bool axisConst = GetRotateAxisInput().IsConstant(),
         angleConst = GetRotateAmountInput().IsConstant(),
         inputConst = GetToRotateInput().IsConstant();

    std::string funcInvoker = GetName() + "_rotFunc(";

    if (axisConst && angleConst && inputConst)
        funcInvoker += ")";
    else if (angleConst && inputConst)
        funcInvoker += GetRotateAxisInput().GetValue();
    else if (axisConst && inputConst)
        funcInvoker += GetRotateAmountInput().GetValue();
    else if (axisConst && angleConst)
        funcInvoker += GetToRotateInput().GetValue();
    else if (angleConst)
        funcInvoker += GetToRotateInput().GetValue() + ", " + GetRotateAxisInput().GetValue();
    else if (axisConst)
        funcInvoker += GetToRotateInput().GetValue() + ", " + GetRotateAmountInput().GetValue();
    else if (inputConst)
        funcInvoker += GetRotateAxisInput().GetValue() + ", " + GetRotateAmountInput().GetValue();
    else funcInvoker += GetToRotateInput().GetValue() + ", " + GetRotateAxisInput().GetValue() + ", " + GetRotateAmountInput().GetValue();
    
    funcInvoker += ")";


    outCode += "\tvec3 " + GetOutputName(0) + " = " + funcInvoker + ";\n";
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