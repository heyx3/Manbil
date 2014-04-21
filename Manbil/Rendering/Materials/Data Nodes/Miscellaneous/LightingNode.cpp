#include "LightingNode.h"


void LightingNode::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
        outDecls.insert(outDecls.end(), std::string() +
"float " + GetFuncName() + "(float amb, float diff, float spec, float specInts,                                         \n\
                     vec3 lightDir, vec3 surfNormal, vec3 fragToCamNormal)                                      \n\
{                                                                                                               \n\
    float dotted = max(dot(-surfNormal, lightDir), 0.0);                                                        \n\
    vec3 lightReflect = normalize(reflect(lightDir, surfNormal));                                               \n\
    " +
      (!GetSpecularInput().IsConstant(0.0f) ?
"    float specFactor = max(0.0, dot(fragToCamNormal, lightReflect));                                     \n\
    specFactor = pow(specFactor, specularIntensity);                                                      \n\
    return (amb + (diff * dotted)) + (spec * specFactor);\n" :
"    return amb + (diff * dotted);\n") +
"}\n\n");
}

void LightingNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tfloat " + GetOutputName(0) + " = " + GetFuncName() + "(" +
        GetAmbientInput().GetValue() + ", " +
        GetDiffuseInput().GetValue() + ", " +
        GetSpecularInput().GetValue() + ", " +
        GetSpecularIntensityInput().GetValue() + ", " +
        GetLightDirInput().GetValue() + ", " +
        GetSurfaceNormalInput().GetValue() + ", " +
        GetCameraPosInput().GetValue() + " - " + GetSurfacePosInput().GetValue() +
        ");\n";
}

std::vector<DataLine> LightingNode::MakeInputVector(const DataLine & amb, const DataLine & diff, const DataLine & spec, const DataLine & specIntense,
                                                    const DataLine & camPos, const DataLine & surfPos,
                                                    const DataLine & surfNormal, const DataLine & lightDir)
{
    std::vector<DataLine> ret;
    ret.insert(ret.end(), amb);
    ret.insert(ret.end(), diff);
    ret.insert(ret.end(), spec);
    ret.insert(ret.end(), specIntense);
    ret.insert(ret.end(), camPos);
    ret.insert(ret.end(), surfNormal);
    ret.insert(ret.end(), lightDir);
    ret.insert(ret.end(), surfPos);
    return ret;
}