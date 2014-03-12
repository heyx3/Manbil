#include "PostProcessData.h"

const std::string PostProcessEffect::ColorSampler = "u_colorTex",
                  PostProcessEffect::DepthSampler = "u_depthTex";


void ContrastEffect::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
    switch (Strength)
    {
    case Strengths::S_Light:
        outDecls.insert(outDecls.end(),
"vec3 smoothstep" + std::to_string(GetUniqueID()) + "(vec3 interp)\n\
{                                                                 \n\
    const vec3 zero = vec3(0.0, 0.0, 0.0),                        \n\
               one = vec3(1.0, 1.0, 1.0);                         \n\
    for (int i = 0; i < " + std::to_string(Iterations) + "; ++i)  \n\
    {                                                             \n\
        interp = smoothstep(zero, one, interp);                   \n\
    }                                                             \n\
    return interp;                                                \n\
}\n");
        break;
    case Strengths::S_Heavy:
        outDecls.insert(outDecls.end(),
"vec3 superSmoothstep" + std::to_string(GetUniqueID()) + "(vec3 interp)                   \n\
{                                                                                         \n\
    const vec3 zero = vec3(0.0, 0.0, 0.0),                                                \n\
               one = vec3(1.0, 1.0, 1.0);                                                 \n\
    for (int i = 0; i < " + std::to_string(Iterations) + "; ++i)                          \n\
    {                                                                                     \n\
        interp = interp * interp * interp * (10.0 + (interp * (-15.0 + (interp * 6.0)))); \n\
    }                                                                                     \n\
    return interp;                                                                        \n\
}\n");
        break;

    default: assert(false);
    }
}


void ContrastEffect::WriteMyOutputs(std::string & strOut) const
{
    std::string func = "INVALID_SMOOTH_FUNC";
    switch (Strength)
    {
    case Strengths::S_Light: func = "smoothstep"; break;
    case Strengths::S_Heavy: func = "superSmoothstep"; break;

    default: assert(false);
    }

    func += std::to_string(GetUniqueID());


    strOut += "\tvec3 " + GetOutputName(0) + " = " + func + "(" + GetColorInput().GetValue() + ");\n";
    strOut += "\tfloat " + GetOutputName(1) + " = " + GetDepthInput().GetValue() + ";\n";
}


void FogEffect::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
    outDecls.insert(outDecls.end(),
"vec3 blendWithFog" + std::to_string(GetUniqueID()) + "(vec3 colorIn, float depthIn)\n\
{                                                          \n\
    float fogLerp = pow(1.0 - depthIn, " + DataLine(VectorF(Dropoff)).GetValue() + ");                 \n\
    return mix(" + DataLine(VectorF(FogColor)).GetValue() + ", " +
               "colorIn, fogLerp);                         \n\
                                                           \n\
}                                                          \n\
");
}

void FogEffect::WriteMyOutputs(std::string & strOut) const
{
    strOut += "\tvec3 " + GetOutputName(0) + " = blendWithFog" + std::to_string(GetUniqueID()) + "(" + GetColorInput().GetValue() + ", " +
                                                                                                       GetDepthInput().GetValue() + ");\n";
    strOut += "\tfloat " + GetOutputName(1) + " = " + GetDepthInput().GetValue() + ";\n";
    //strOut += "\t " + GetOutputName(0) + " = vec3(pow(" + GetDepthInput().GetValue() + ", 1.0));\n";
}