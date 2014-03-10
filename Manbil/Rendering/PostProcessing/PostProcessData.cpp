#include "PostProcessData.h"

const std::string PostProcessEffect::ColorAndDepthSampler = "u_colorDepthTex";


std::vector<DataLine> PostProcessEffect::MakeVector(DataLine colDepthIn, const std::vector<DataLine> & otherInputs)
{
    std::vector<DataLine> ret = otherInputs;

    ret.insert(ret.end(), DataLine(DataNodePtr(new SwizzleNode(colDepthIn,
        SwizzleNode::Components::C_X,
        SwizzleNode::Components::C_Y,
        SwizzleNode::Components::C_Z)), 0));
    ret.insert(ret.end(), DataLine(DataNodePtr(new SwizzleNode(colDepthIn, SwizzleNode::Components::C_W)), 0));

    return ret;
}


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
        interp = interp * interp * interp * (10.0 + (interp * (-15.0 + (interp * 6.0))))) \n\
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


    strOut += "\tvec4 " + GetOutputName(0) + " = vec4(" + func + "(" + GetColorInput().GetValue() + "), " + GetDepthInput().GetValue() + ");";
}