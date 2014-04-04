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
}


void FogEffect::WriteMyOutputs(std::string & strOut) const
{
    std::string innerLerpVal = (GetFogThicknessInput().IsConstant(1.0f) ?
                                    std::string() + GetDepthInput().GetValue() :
                                    std::string() + "(" + GetDepthInput().GetValue() + " * " + GetFogThicknessInput().GetValue() + ")");
    std::string lerpVal = (GetDropoffInput().IsConstant(1.0f) ?
                              std::string("(1.0 - ") + innerLerpVal + ")" :
                              std::string("pow(1.0 - ") + innerLerpVal + ", " + GetDropoffInput().GetValue() + ")");
    strOut += "\n\
    vec3 " + GetOutputName(0) + " = mix(" + GetFogColorInput().GetValue() + ", " +
                                        GetColorInput().GetValue() + ", \n\
                                        " + lerpVal + ");\n";
}


void GaussianBlurEffect::WriteMyOutputs(std::string & outStr) const
{

}

std::vector<DataLine> GaussianBlurEffect::BuildInputs(void)
{
    std::vector<DataLine> ret;

    //TODO: Calculate the correct lookup locations for the color sampler, either with constants or DataNodes.

    return ret;
}