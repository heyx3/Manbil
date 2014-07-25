#include "LightingNode.h"


LightingNode::LightingNode(const DataLine & surfaceWorldPos, const DataLine & surfaceWorldNormal, const DataLine & lightDir,
                           DataLine ambient, DataLine diffuse, DataLine specular, DataLine specIntensity, DataLine camPos)
    : DataNode(MakeInputVector(ambient, diffuse, specular, specIntensity, camPos, surfaceWorldPos, surfaceWorldNormal, lightDir),
               MakeVector(1))
{
    Assert(ambient.GetDataLineSize() == 1, std::string() + "Ambient input must be size 1; is size " + ToString(ambient.GetDataLineSize()));
    Assert(diffuse.GetDataLineSize() == 1, std::string() + "Diffuse input must be size 1; is size " + ToString(diffuse.GetDataLineSize()));
    Assert(specular.GetDataLineSize() == 1, std::string() + "Specular input must be size 1; is size " + ToString(specular.GetDataLineSize()));
    Assert(specIntensity.GetDataLineSize() == 1, std::string() + "Specular intensity input must be size 1; is size " + ToString(specIntensity.GetDataLineSize()));

    Assert(camPos.GetDataLineSize() == 3, std::string() + "CamPos input must be size 3; is size " + ToString(camPos.GetDataLineSize()));
    Assert(surfaceWorldPos.GetDataLineSize() == 3, std::string() + "SurfaceWorldPos input must be size 3; is size " + ToString(surfaceWorldPos.GetDataLineSize()));
    Assert(surfaceWorldNormal.GetDataLineSize() == 3, std::string() + "SurfaceWorldNormal input must be size 3; is size " + ToString(surfaceWorldNormal.GetDataLineSize()));
    Assert(lightDir.GetDataLineSize() == 3, std::string() + "LightDir input must be size 3; is size " + ToString(lightDir.GetDataLineSize()));
}

void LightingNode::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
    //TODO: If no specular component, remove the "fragToCamNormal" argument.
        outDecls.insert(outDecls.end(), std::string() +
"float " + GetFuncName() + "(float amb, float diff, float spec, float specInts,                                 \n\
                     vec3 lightDir, vec3 surfNormal, vec3 fragToCamNormal)                                      \n\
{                                                                                                               \n\
    float dotted = max(dot(surfNormal, -lightDir), 0.0);                                                        \n\
                                                                                                                \n\
    " +
      (!GetSpecularInput().IsConstant(0.0f) ?
    "vec3 lightReflect = normalize(reflect(lightDir, surfNormal));                                              \n\
    float specFactor = max(0.0, dot(fragToCamNormal, lightReflect));                                            \n\
    specFactor = pow(specFactor, specInts);                                                                     \n\
                                                                                                                \n\
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
        "normalize(" + GetCameraPosInput().GetValue() + " - " + GetSurfacePosInput().GetValue() + ")" +
        ");\n";
}

std::vector<DataLine> LightingNode::MakeInputVector(const DataLine & amb, const DataLine & diff, const DataLine & spec, const DataLine & specIntense,
                                                    const DataLine & camPos, const DataLine & surfPos,
                                                    const DataLine & surfNormal, const DataLine & lightDir)
{
    std::vector<DataLine> ret;
    ret.insert(ret.end(), surfPos);
    ret.insert(ret.end(), surfNormal);
    ret.insert(ret.end(), lightDir);
    ret.insert(ret.end(), amb);
    ret.insert(ret.end(), diff);
    ret.insert(ret.end(), spec);
    ret.insert(ret.end(), specIntense);
    ret.insert(ret.end(), camPos);
    return ret;
}