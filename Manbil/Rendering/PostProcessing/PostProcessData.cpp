#include "PostProcessData.h"

#include "../Helper Classes/DrawingQuad.h"



const std::string PostProcessEffect::ColorSampler = "u_colorTex",
                  PostProcessEffect::DepthSampler = "u_depthTex";
std::vector<DataNode::Ptr> * PostProcessEffect::NodeStorage = 0;


void PreparePpeEffectsToBeRead()
{
    ColorTintEffect(DataLine(VectorF(1.0f, 1.0f, 1.0f)), "a");
    ContrastEffect(ContrastEffect::Strengths::S_Light, 1, "a");
    FogEffect(DataLine(VectorF(1.0f)), DataLine(VectorF(1.0f, 1.0f, 1.0f)), DataLine(1.0f), "a");
    GaussianBlurEffect("a");
}


void PostProcessEffect::ChangePreviousEffect(PpePtr newPrevEffect)
{
    PrevEffect = newPrevEffect;

    if (newPrevEffect.get() == 0)
    {
        ReplaceInput(GetInputs().size() - 2, ColorSamplerIn(GetName() + "_"));
    }
    else
    {
        ReplaceInput(GetInputs().size() - 2, DataLine(newPrevEffect->GetName(), GetColorOutputIndex()));
        ReplaceInput(GetInputs().size() - 1, newPrevEffect->GetDepthInput());
    }
}
unsigned int PostProcessEffect::GetOutputSize(unsigned int index) const
{
    Assert(index <= 1, "Output index is greater than 1: " + ToString(index));
    return (index == 0 ? 3 : 1);
}
std::string PostProcessEffect::GetOutputName(unsigned int index) const
{
    Assert(index <= 1, "Output index is greater than 1: " + ToString(index));
    return GetName() + (index == 0 ? "_colorOut" : "_depthOut");
}
std::vector<DataLine> PostProcessEffect::MakeVector(PpePtr prevEffect, std::string namePrefix, const std::vector<DataLine> & otherInputs)
{
    std::vector<DataLine> ret = otherInputs;
    if (prevEffect.get() == 0)
    {
        ret.insert(ret.end(), ColorSamplerIn(namePrefix));
        ret.insert(ret.end(), DepthSamplerIn(namePrefix));
    }
    else
    {
        ret.insert(ret.end(), DataLine(prevEffect->GetName(), GetColorOutputIndex()));
        ret.insert(ret.end(), prevEffect->GetDepthInput());
    }
    return ret;
}


void ContrastEffect::GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const
{
    switch (Strength)
    {
    case Strengths::S_Light:
        outDecls.insert(outDecls.end(),
"vec3 smoothstep_" + GetName() + "(vec3 interp)\n\
{                                                                 \n\
    const vec3 zero = vec3(0.0, 0.0, 0.0),                        \n\
               one = vec3(1.0, 1.0, 1.0);                         \n\
    for (int i = 0; i < " + ToString(Iterations) + "; ++i)  \n\
    {                                                             \n\
        interp = smoothstep(zero, one, interp);                   \n\
    }                                                             \n\
    return interp;                                                \n\
}\n");
        break;
    case Strengths::S_Heavy:
        outDecls.insert(outDecls.end(),
"vec3 superSmoothstep_" + GetName() + "(vec3 interp)                   \n\
{                                                                                         \n\
    const vec3 zero = vec3(0.0, 0.0, 0.0),                                                \n\
               one = vec3(1.0, 1.0, 1.0);                                                 \n\
    for (int i = 0; i < " + ToString(Iterations) + "; ++i)                          \n\
    {                                                                                     \n\
        interp = interp * interp * interp * (10.0 + (interp * (-15.0 + (interp * 6.0)))); \n\
    }                                                                                     \n\
    return interp;                                                                        \n\
}\n");
        break;

    default: Assert(false, std::string() + "Unknown strength amount: " + ToString(Strength));
    }
}

void ContrastEffect::WriteMyOutputs(std::string & strOut) const
{
    std::string func = "INVALID_SMOOTH_FUNC";
    switch (Strength)
    {
        case Strengths::S_Light: func = "smoothstep"; break;
        case Strengths::S_Heavy: func = "superSmoothstep"; break;

        default: Assert(false, "Unknown strength amount: " + ToString(Strength));
    }
    func += GetName();

    strOut += "\tvec3 " + GetOutputName(0) + " = " + func + "(" + GetColorInput().GetValue() + ");\n";
}

bool ContrastEffect::WriteExtraData(DataWriter * writer, std::string & outError) const
{
    std::string strengthStr;
    switch (Strength)
    {
        case S_Light:
            strengthStr = "light";
            break;
        case S_Heavy:
            strengthStr = "heavy";
            break;
        default:
            Assert(false, "Invalid strength value " + ToString(Strength));
            break;
    }
    if (!writer->WriteString(strengthStr, "Strength", outError))
    {
        outError = "Error writing strength value '" + strengthStr + "': " + outError;
        return false;
    }

    if (!writer->WriteUInt(Iterations, "Iterations", outError))
    {
        outError = "Error writing number of iterations " + ToString(Iterations) + ": " + outError;
        return false;
    }

    return true;
}
bool ContrastEffect::ReadExtraData(DataReader * reader, std::string & outError)
{
    MaybeValue<std::string> tryStr = reader->ReadString(outError);
    if (!tryStr.HasValue())
    {
        outError = "Error reading strength value: " + outError;
        return false;
    }

    if (tryStr.GetValue().compare("light") == 0)
        Strength = S_Light;
    else if (tryStr.GetValue().compare("heavy") == 0)
        Strength = S_Heavy;
    else
    {
        outError = "Unknown strength value '" + tryStr.GetValue() + "'";
        return false;
    }

    MaybeValue<unsigned int> tryIterations = reader->ReadUInt(outError);
    if (!tryIterations.HasValue())
    {
        outError = "Error reading number of iterations: " + outError;
        return false;
    }
    Iterations = tryIterations.GetValue();

    return true;
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


void GaussianBlurEffect::OverrideVertexOutputs(std::vector<ShaderOutput> & vertOuts) const
{
    if (CurrentPass == 1 || CurrentPass == 4) return;

    DataLine uvs(VertexInputNode::GetInstance()->GetName(), 1);

    vertOuts.clear();
    for (unsigned int i = 0; i < 15; ++i)
    {
        const float increment = 0.0004f;
        DataLine uvAdded = (CurrentPass == 2 ?
                               VectorF(increment * (float)(i - 7), 0.0f) :
                               VectorF(0.0f, increment * (float)(i - 7), 0.0f));
        std::string nodeName = GetName() + "_add" + ToString(i);
        NodeStorage->insert(NodeStorage->end(), Ptr(new AddNode(uvAdded, uvs, nodeName)));
        vertOuts.insert(vertOuts.end(), ShaderOutput("outUV_" + ToString(i), DataLine(nodeName)));
    }

    /*
    typedef RenderingChannels rc;
    DataLine uvs(Ptr(new VertexInputNode(DrawingQuad::GetAttributeData())), 1);

    if (CurrentPass == 2)
    {
        channels[rc::RC_VERTEX_OUT_0] = DataLine(Ptr(new AddNode(DataLine(Vector2f(-0.0028f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_1] = DataLine(Ptr(new AddNode(DataLine(Vector2f(-0.0024f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_2] = DataLine(Ptr(new AddNode(DataLine(Vector2f(-0.0020f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_3] = DataLine(Ptr(new AddNode(DataLine(Vector2f(-0.0016f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_4] = DataLine(Ptr(new AddNode(DataLine(Vector2f(-0.0012f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_5] = DataLine(Ptr(new AddNode(DataLine(Vector2f(-0.0008f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_6] = DataLine(Ptr(new AddNode(DataLine(Vector2f(-0.0004f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_7] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0004f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_8] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0008f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_9] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0012f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_10] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0016f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_11] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0020f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_12] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0024f, 0.0f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_13] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0028f, 0.0f)), uvs)), 0);

        channels[rc::RC_VERTEX_OUT_14] = uvs;
    }
    else
    {
        Assert(CurrentPass == 3, std::string() + "Invalid pass number: " + ToString(CurrentPass));

        channels[rc::RC_VERTEX_OUT_0] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, -0.0028f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_1] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, -0.0024f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_2] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, -0.0020f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_3] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, -0.0016f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_4] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, -0.0012f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_5] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, -0.0008f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_6] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, -0.0004f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_7] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, 0.0004f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_8] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, 0.0008f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_9] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, 0.0012f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_10] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, 0.0016f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_11] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, 0.0020f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_12] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, 0.0024f)), uvs)), 0);
        channels[rc::RC_VERTEX_OUT_13] = DataLine(Ptr(new AddNode(DataLine(Vector2f(0.0f, 0.0028f)), uvs)), 0);

        channels[rc::RC_VERTEX_OUT_14] = uvs;
    }
    */
}
void GaussianBlurEffect::WriteMyOutputs(std::string & outStr) const
{
    if (CurrentPass == 1 || CurrentPass == 4) return;
    
    if (CurrentShader == ShaderHandler::SH_Fragment_Shader)
    {
        std::string output = GetOutputName(GetColorOutputIndex());
        outStr += "\n\t//Use built-in interpolation step to speed up texture sampling.\n\
    vec3 " + output + " = vec3(0.0);\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_0).xyz  * 0.0044299121055113265;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_1).xyz  * 0.00895781211794;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_2).xyz  * 0.0215963866053;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_3).xyz  * 0.0443683338718;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_4).xyz  * 0.0776744219933;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_5).xyz  * 0.115876621105;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_6).xyz  * 0.147308056121;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_7).xyz  * 0.159576912161;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_8).xyz  * 0.147308056121;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_9).xyz  * 0.115876621105;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_10).xyz * 0.0776744219933;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_11).xyz * 0.0443683338718;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_12).xyz * 0.0215963866053;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_13).xyz * 0.00895781211794;\n\
    " + output + " += texture2D(" + ColorSampler + ", outUV_14).xyz * 0.0044299121055113265;\n";
    }
}