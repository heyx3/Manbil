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


void GaussianBlurEffect::GetMyFunctionDeclarations(std::vector<std::string> & decls) const
{
    decls.insert(decls.end(), "\tout vec2 out_blurTexCoords[14];\n");
}
void GaussianBlurEffect::WriteMyOutputs(std::string & outStr) const
{
    if (CurrentPass == 1 || CurrentPass == 4) return;

    std::string outTC = "out_blurTexCoords";

    std::string uv = MaterialConstants::OutUV;

    if (GetShaderType() == DataNode::Shaders::SH_Vertex_Shader)
    {
        switch (CurrentPass)
        {
        case 2:
            outStr += "\n\t//Use built-in interpolation step to speed up texture sampling.\n\
    " + outTC + "[ 0] = " + uv + "vec2(-0.028, 0.0);\n\
    " + outTC + "[ 1] = " + uv + "vec2(-0.024, 0.0);\n\
    " + outTC + "[ 2] = " + uv + "vec2(-0.020, 0.0);\n\
    " + outTC + "[ 3] = " + uv + "vec2(-0.016, 0.0);\n\
    " + outTC + "[ 4] = " + uv + "vec2(-0.012, 0.0);\n\
    " + outTC + "[ 5] = " + uv + "vec2(-0.008, 0.0);\n\
    " + outTC + "[ 6] = " + uv + "vec2(-0.004, 0.0);\n\
    " + outTC + "[ 7] = " + uv + "vec2( 0.004, 0.0);\n\
    " + outTC + "[ 8] = " + uv + "vec2( 0.008, 0.0);\n\
    " + outTC + "[ 9] = " + uv + "vec2( 0.012, 0.0);\n\
    " + outTC + "[11] = " + uv + "vec2( 0.016, 0.0);\n\
    " + outTC + "[10] = " + uv + "vec2( 0.020, 0.0);\n\
    " + outTC + "[11] = " + uv + "vec2( 0.024, 0.0);\n\
    " + outTC + "[12] = " + uv + "vec2( 0.028, 0.0);\n\n";
            break;
        case 3:
            outStr += "\n\t//Use built-in interpolation step to speed up texture sampling.\n\
    " + outTC + "[ 0] = " + uv + "vec2(0.0, -0.028);\n\
    " + outTC + "[ 1] = " + uv + "vec2(0.0, -0.024);\n\
    " + outTC + "[ 2] = " + uv + "vec2(0.0, -0.020);\n\
    " + outTC + "[ 3] = " + uv + "vec2(0.0, -0.016);\n\
    " + outTC + "[ 4] = " + uv + "vec2(0.0, -0.012);\n\
    " + outTC + "[ 5] = " + uv + "vec2(0.0, -0.008);\n\
    " + outTC + "[ 6] = " + uv + "vec2(0.0, -0.004);\n\
    " + outTC + "[ 7] = " + uv + "vec2(0.0,  0.004);\n\
    " + outTC + "[ 8] = " + uv + "vec2(0.0,  0.008);\n\
    " + outTC + "[ 9] = " + uv + "vec2(0.0,  0.012);\n\
    " + outTC + "[11] = " + uv + "vec2(0.0,  0.016);\n\
    " + outTC + "[10] = " + uv + "vec2(0.0,  0.020);\n\
    " + outTC + "[11] = " + uv + "vec2(0.0,  0.024);\n\
    " + outTC + "[12] = " + uv + "vec2(0.0,  0.028);\n\n";
            break;
        default: assert(false);
        }
    }
    else
    {
        assert(GetShaderType() == DataNode::Shaders::SH_Fragment_Shader);

        std::string output = GetOutputName(GetColorOutputIndex());
        outStr += "\n\t//Use built-in interpolation step to speed up texture sampling.\n\
    " + output + " = vec3(0.0);\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 0]) * 0.0044299121055113265;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 1]) * 0.00895781211794;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 2]) * 0.0215963866053;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 3]) * 0.0443683338718;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 4]) * 0.0776744219933;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 5]) * 0.115876621105;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 6]) * 0.147308056121;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + MaterialConstants::OutUV + ") * 0.159576912161;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 7]) * 0.147308056121;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 8]) * 0.115876621105;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[ 9]) * 0.0776744219933;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[10]) * 0.0443683338718;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[11]) * 0.0215963866053;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[12]) * 0.00895781211794;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "[13]) * 0.0044299121055113265;\n";
    }
}