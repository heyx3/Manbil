#include "LightingNode.h"


MAKE_NODE_READABLE_CPP(LightingNode, Vector3f(), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 0.0f, -1.0f), "", 0.0f, 0.0f, 0.0f, 0.0f, Vector3f())


unsigned int LightingNode::GetOutputSize(unsigned int index) const
{
    return 1;
}
std::string LightingNode::GetOutputName(unsigned int outputIndex) const
{
    return GetName() + "_brightness";
}


LightingNode::LightingNode(const DataLine & surfaceWorldPos, const DataLine & surfaceWorldNormal, const DataLine & lightDir,
                           std::string name, DataLine ambient, DataLine diffuse,
                           DataLine specular, DataLine specIntensity, DataLine camPos)
    : DataNode(MakeInputVector(ambient, diffuse, specular, specIntensity,
                               camPos, surfaceWorldPos, surfaceWorldNormal, lightDir), name)
{

}

void LightingNode::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
        std::string func = std::string() +
"float " + GetFuncName() + "(float amb, float diff, float spec, float specInts,    \n\
                     vec3 lightDir, vec3 surfNormal";
        if (!GetSpecularInput().IsConstant(0.0f))
            func += ", vec3 fragToCamNormal";
        func += std::string() + ")                                                 \n\
{                                                                                  \n\
    float dotted = max(dot(surfNormal, -lightDir), 0.0);                           \n\
                                                                                   \n\
" +
      (!GetSpecularInput().IsConstant(0.0f) ?
"    vec3 lightReflect = normalize(reflect(lightDir, surfNormal));                 \n\
    float specFactor = max(0.0, dot(fragToCamNormal, lightReflect));               \n\
    specFactor = pow(specFactor, specInts);                                        \n\
                                                                                   \n\
    return (amb + (diff * dotted)) + (spec * specFactor);\n" :
"    return amb + (diff * dotted);\n") +
"}\n\n";

    outDecls.insert(outDecls.end(), func);
}

void LightingNode::WriteMyOutputs(std::string & outCode) const
{
    outCode += "\tfloat " + GetOutputName(0) + " = " + GetFuncName() + "(" +
        GetAmbientInput().GetValue() + ", " +
        GetDiffuseInput().GetValue() + ", " +
        GetSpecularInput().GetValue() + ", " +
        GetSpecularIntensityInput().GetValue() + ", " +
        GetLightDirInput().GetValue() + ", " +
        GetSurfaceNormalInput().GetValue();
    if (!GetSpecularInput().IsConstant(0.0f))
        outCode += ", normalize(" + GetCameraPosInput().GetValue() + " - " + GetSurfacePosInput().GetValue() + ")";
    outCode += ");\n";
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

std::string LightingNode::GetInputDescription(unsigned int index) const
{
    switch (index)
    {
        case 0: return "worldSurfacePos";
        case 1: return "worldSurfaceNormal";
        case 2: return "lightDirection";
        case 3: return "ambientStrength";
        case 4: return "diffuseStrength";
        case 5: return "specularStrength";
        case 6: return "specularIntensity";
        case 7: return "worldCameraPos";
        default:
            Assert(false, "Invalid input index " + ToString(index));
            return "BAD_INPUT_INDEX";
    }
}


void LightingNode::AssertMyInputsValid(void) const
{
    Assert(GetAmbientInput().GetSize() == 1, "Ambient input must be size 1; is size " + ToString(GetAmbientInput().GetSize()));
    Assert(GetDiffuseInput().GetSize() == 1, "Diffuse input must be size 1; is size " + ToString(GetDiffuseInput().GetSize()));
    Assert(GetSpecularInput().GetSize() == 1, "Specular input must be size 1; is size " + ToString(GetSpecularInput().GetSize()));
    Assert(GetSpecularIntensityInput().GetSize() == 1, "Specular intensity input must be size 1; is size " + ToString(GetSpecularIntensityInput().GetSize()));

    Assert(GetCameraPosInput().GetSize() == 3, "CamPos input must be size 3; is size " + ToString(GetCameraPosInput().GetSize()));
    Assert(GetSurfacePosInput().GetSize() == 3, "SurfaceWorldPos input must be size 3; is size " + ToString(GetSurfacePosInput().GetSize()));
    Assert(GetSurfaceNormalInput().GetSize() == 3, "SurfaceWorldNormal input must be size 3; is size " + ToString(GetSurfaceNormalInput().GetSize()));
    Assert(GetLightDirInput().GetSize() == 3, "LightDir input must be size 3; is size " + ToString(GetLightDirInput().GetSize()));
}