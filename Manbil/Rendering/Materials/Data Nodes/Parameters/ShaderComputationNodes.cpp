#include "ShaderComputationNodes.h"


void ObjectPosToScreenPosCalcNode::WriteMyOutputs(std::string & outCode) const
{
    std::string objPos = GetInputs()[0].GetValue();
    std::string homgPosName = GetOutputName(GetHomogenousPosOutputIndex()),
                normalPosName = GetOutputName(GetPosOutputIndex());

    outCode += "\tvec4 " + homgPosName + " = (" + MaterialConstants::WVPMatName + " * vec4(" + objPos + ", 1.0));\n";
    outCode += "\tvec3 " + normalPosName + " = " + homgPosName + ".xyz / " + homgPosName + ".w;\n";
}

void ObjectNormalToScreenNormalCalcNode::WriteMyOutputs(std::string & outCode) const
{
    std::string objNorm = GetInputs()[0].GetValue();
    std::string outName = GetOutputName(0);

    outCode += "\tvec3 " + outName + " = (" + MaterialConstants::WVPMatName + " * vec4(" + objNorm + ", 0.0)).xyz\n";
}

void ObjectPosToWorldPosCalcNode::WriteMyOutputs(std::string & outCode) const
{
    std::string objPos = GetInputs()[0].GetValue();
    std::string output = GetOutputName(0),
                tempOut = output + "_temp";

    outCode += "\tvec4 " + tempOut + " = (" + MaterialConstants::WorldMatName + " * vec4(" + objPos + ", 0.0));\n";
    outCode += "\tvec3 " + output + " = " + tempOut + ".xyz / " + tempOut + ".w;\n";
}

void ObjectNormalToWorldNormalCalcNode::WriteMyOutputs(std::string & outCode) const
{
    DataNodePtr objPos = GetInputs()[0].GetDataNodeValue();
    std::string outName = GetOutputName(0);

    outCode += "\tvec3 " + outName + " = (" + MaterialConstants::WorldMatName + " * vec4(" + objPos->GetOutputName(0) + ", 0.0)).xyz\n";
}