#include "MoreShaderInNodes.h"


void ScreenPosNode::WriteMyOutputs(std::string & outCode) const
{
    const DataNodePtr & objPos = GetInputs()[0].GetDataNodeValue();
    std::string outName = GetOutputName(0),
                tempName = "temp_" + outName;

    outCode += "\tvec4 " + tempName + " = (" + MaterialConstants::WVPMatName + " * vec4(" + objPos->GetOutputName(0) + ", 1.0))\n";
    outCode += "\tvec3 " + outName + " = " + tempName + ".xyz / " + tempName + ".w;\n";
}

void ScreenNormalNode::WriteMyOutputs(std::string & outCode) const
{
    const DataNodePtr & objPos = GetInputs()[0].GetDataNodeValue();
    std::string outName = GetOutputName(0);

    outCode += "\tvec3 " + outName + " = (" + MaterialConstants::WVPMatName + " * vec4(" + objPos->GetOutputName(0) + ", 0.0)).xyz\n";
}