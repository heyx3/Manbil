#include "ShaderComputationNodes.h"

#include "../../MaterialData.h"


void ObjectPosToScreenPosCalcNode::WriteMyOutputs(std::string & outCode) const
{
    std::string homgPosName = GetOutputName(GetHomogenousPosOutputIndex()),
                normalPosName = GetOutputName(GetPosOutputIndex());

    outCode += "\tvec4 " + homgPosName + " = (" + MaterialConstants::WVPMatName + " * vec4(" + GetInputs()[0].GetValue() + ", 1.0));\n";
    outCode += "\tvec3 " + normalPosName + " = " + homgPosName + ".xyz / " + homgPosName + ".w;\n";
}

void ObjectNormalToScreenNormalCalcNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec3 " + GetOutputName(0) + " = (" + MaterialConstants::WVPMatName + " * vec4(" + GetInputs()[0].GetValue() + ", 0.0)).xyz;\n";
}

void ObjectPosToWorldPosCalcNode::WriteMyOutputs(std::string & outCode) const
{
    std::string output = GetOutputName(0),
                tempOut = output + "_temp";

    outCode += "\tvec4 " + tempOut + " = (" + MaterialConstants::WorldMatName + " * vec4(" + GetInputs()[0].GetValue() + ", 1.0));\n";
    outCode += "\tvec3 " + output + " = " + tempOut + ".xyz / " + tempOut + ".w;\n";
}

void ObjectNormalToWorldNormalCalcNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec3 " + GetOutputName(0) + " = (" + MaterialConstants::WorldMatName + " * vec4(" + GetInputs()[0].GetValue() + ", 0.0)).xyz;\n";
}

void WorldPosToScreenPosCalcNode::WriteMyOutputs(std::string & outCode) const
{
    std::string homgOut = GetOutputName(GetHomogenousPosOutputIndex()),
                posOut = GetOutputName(GetPosOutputIndex());
    outCode += "\tvec3 " + homgOut + " = (" + MaterialConstants::ViewProjMatName + " * vec4(" + GetInputs()[0].GetValue() + ", 1.0));\n";
    outCode += "\tvec3 " + posOut + " = " + homgOut + ".xyz / " + homgOut + ".w;\n";
}
void WorldNormalToScreenNormalCalcNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tvec3 " + GetOutputName(0) + " = (" + MaterialConstants::ViewProjMatName + " * vec4(" + GetInputs()[0].GetValue() + ", 0.0)).xyz;\n";
}