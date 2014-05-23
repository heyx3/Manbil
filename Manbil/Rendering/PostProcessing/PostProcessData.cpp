#include "PostProcessData.h"

#include "../Helper Classes/DrawingQuad.h"



const std::string PostProcessEffect::ColorSampler = "u_colorTex",
                  PostProcessEffect::DepthSampler = "u_depthTex";
int PostProcessEffect::VertexInputUVIndex = -1;

void PostProcessEffect::ChangePreviousEffect(PpePtr newPrevEffect)
{
    PrevEffect = newPrevEffect;

    if (newPrevEffect.get() == 0)
    {
        ReplaceInput(GetInputs().size() - 2, ColorSamplerIn());
    }
    else
    {
        ReplaceInput(GetInputs().size() - 2, DataLine(newPrevEffect, GetColorOutputIndex()));
        ReplaceInput(GetInputs().size() - 1, newPrevEffect->GetDepthInput());
    }
}
std::string PostProcessEffect::GetOutputName(unsigned int index) const
{
    Assert(index == GetDepthOutputIndex(), std::string() + "Output index is something other than " + ToString(GetDepthOutputIndex()) + ": " + ToString(index));
    if (PrevEffect.get() == 0)
        return GetDepthInput().GetValue();
    else return PrevEffect->GetOutputName(index);
}
std::vector<DataLine> PostProcessEffect::MakeVector(PpePtr prevEffect, const std::vector<DataLine> & otherInputs)
{
    std::vector<DataLine> ret = otherInputs;
    if (prevEffect.get() == 0)
    {
        ret.insert(ret.end(), ColorSamplerIn());
        ret.insert(ret.end(), DepthSamplerIn());
    }
    else
    {
        ret.insert(ret.end(), DataLine(prevEffect, GetColorOutputIndex()));
        ret.insert(ret.end(), prevEffect->GetDepthInput());
    }
    return ret;
}


std::string ColorTintEffect::GetOutputName(unsigned int index) const
{
    Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));
    return (index == 0 ?
            (GetName() + std::to_string(GetUniqueID()) + "_tinted") :
            PostProcessEffect::GetOutputName(index));
}


std::string ContrastEffect::GetOutputName(unsigned int index) const
{
    Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));
    return (index == 0 ?
            (GetName() + std::to_string(GetUniqueID()) + "_upContrast") :
            PostProcessEffect::GetOutputName(index));
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
        interp = interp * interp * interp * (10.0 + (interp * (-15.0 + (interp * 6.0)))); \n\
    }                                                                                     \n\
    return interp;                                                                        \n\
}\n");
        break;

    default: Assert(false, std::string() + "Unknown strength amount: " + std::to_string(Strength));
    }
}

void ContrastEffect::WriteMyOutputs(std::string & strOut) const
{
    std::string func = "INVALID_SMOOTH_FUNC";
    switch (Strength)
    {
        case Strengths::S_Light: func = "smoothstep"; break;
        case Strengths::S_Heavy: func = "superSmoothstep"; break;

        default: Assert(false, std::string() + "Unknown strength amount: " + std::to_string(Strength));
    }

    func += std::to_string(GetUniqueID());


    strOut += "\tvec3 " + GetOutputName(0) + " = " + func + "(" + GetColorInput().GetValue() + ");\n";
}


std::string FogEffect::GetOutputName(unsigned int index) const
{
    Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));
    return (index == 0 ?
            (GetName() + std::to_string(GetUniqueID()) + "_foggy") :
            PostProcessEffect::GetOutputName(index));
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


std::string GaussianBlurEffect::GetOutputName(unsigned int index) const
{
    Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));
    if (index == 1) return PostProcessEffect::GetOutputName(index);

    switch (CurrentPass)
    {
    case 1:
    case 4:
        return GetColorInput().GetValue();
    case 2:
    case 3:
        return GetName() + std::to_string(GetUniqueID()) + "_blurred";

    default: Assert(false, std::string() + "Only supports passes 1-4, not pass " + ToString(CurrentPass));
    }

    return "ERROR DANGER DANGER";
}
void GaussianBlurEffect::OverrideVertexOutputs(std::unordered_map<RenderingChannels, DataLine> & channels) const
{
    if (CurrentPass == 1 || CurrentPass == 4) return;

    typedef RenderingChannels rc;
    DataLine uvs(DataNodePtr(new VertexInputNode(DrawingQuad::GetAttributeData())), VertexInputUVIndex);

    if (CurrentPass == 2)
    {
        channels[rc::RC_VERTEX_OUT_1] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(-0.0028f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_2] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(-0.0024f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_3] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(-0.0020f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_4] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(-0.0016f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_5] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(-0.0012f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_6] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(-0.0008f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_7] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(-0.0004f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_8] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0004f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_9] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0008f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_10] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0012f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_11] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0016f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_12] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0020f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_13] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0024f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_14] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0028f, 0.0f)), uvs)), 0);

        channels[rc::RC_VERTEX_OUT_15] = uvs;
    }
    else
    {
        Assert(CurrentPass == 3, std::string() + "Invalid pass number: " + ToString(CurrentPass));

        channels[rc::RC_VERTEX_OUT_1] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, -0.0028f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_2] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, -0.0024f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_3] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, -0.0020f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_4] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, -0.0016f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_5] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, -0.0012f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_6] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, -0.0008f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_7] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, -0.0004f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_8] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, 0.0004f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_9] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, 0.0008f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_10] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, 0.0012f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_11] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, 0.0016f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_12] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, 0.0020f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_13] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, 0.0024f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_14] = DataLine(DataNodePtr(new AddNode(DataLine(Vector2f(0.0f, 0.0028f)), uvs)), 0);

        channels[rc::RC_VERTEX_OUT_15] = uvs;
    }
}
void GaussianBlurEffect::WriteMyOutputs(std::string & outStr) const
{
    if (CurrentPass == 1 || CurrentPass == 4) return;
    
    std::string outTC = MaterialConstants::VertexOutNameBase;
    if (GetShaderType() == DataNode::Shaders::SH_Fragment_Shader)
    {
        std::string output = GetOutputName(GetColorOutputIndex());
        outStr += "\n\t//Use built-in interpolation step to speed up texture sampling.\n\
    vec3 " + output + " = vec3(0.0);\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "1).xyz  * 0.0044299121055113265;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "2).xyz  * 0.00895781211794;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "3).xyz  * 0.0215963866053;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "4).xyz  * 0.0443683338718;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "5).xyz  * 0.0776744219933;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "6).xyz  * 0.115876621105;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "7).xyz  * 0.147308056121;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "15).xyz * 0.159576912161;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "8).xyz  * 0.147308056121;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "9).xyz  * 0.115876621105;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "10).xyz  * 0.0776744219933;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "11).xyz * 0.0443683338718;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "12).xyz * 0.0215963866053;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "13).xyz * 0.00895781211794;\n\
    " + output + " += texture2D(" + ColorSampler + ", " + outTC + "14).xyz * 0.0044299121055113265;\n";
    }
}