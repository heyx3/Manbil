#pragma once

#include <string>
#include "../Materials/Data Nodes/DataNodeIncludes.h"



//Abstract class representing a post-process effect as a DataNode.
//Outputs a vec4: the pixel color in xyz, and the pixel depth in w.
class PostProcessEffect : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "unknownPostProcessEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override { assert(index < GetOutputs().size()); return GetName() + std::to_string(GetUniqueID()) + "_UNKNOWN" + std::to_string(index); }

    //The name of the color/depth texture sampler uniform.
    static const std::string ColorAndDepthSampler;


    //The number of passes needed to do this effect.
    unsigned int NumbPasses;
    //The current pass. Used when generating GLSL code.
    unsigned int CurrentPass;

    PostProcessEffect(DataLine colorDepthIn, std::vector<DataLine> otherInputs, unsigned int numbPasses = 1)
        : DataNode(MakeVector(colorDepthIn, otherInputs), DataNode::MakeVector(4)), NumbPasses(numbPasses), CurrentPass(0)
    {
        assert(colorDepthIn.GetDataLineSize() == 4 &&
               GetColorInput().GetDataLineSize() == 3 &&
               GetDepthInput().GetDataLineSize() == 1);
    }


protected:

    //Returns a DataLine that samples the color/depth texture.
    //This should be the value for the first post-process effect's "colorDepthIn" constructor argument.
    //Subsequent effects should use the previous effect's color/depth output.
    static DataLine ColorDepthSamplerIn(void)
    {
        return DataLine(DataNodePtr(new TextureSampleNode(ColorAndDepthSampler)),
                                    TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllChannels));
    } 

    //Gets the color input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetColorInput(void) const { return GetInputs()[GetInputs().size() - 2]; }
    //Gets the depth input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetDepthInput(void) const { return GetInputs()[GetInputs().size() - 1]; }

private:

    //Creates the input std::vector for this node (by appending the color and depth inputs to the given vector).
    static std::vector<DataLine> MakeVector(DataLine colDepthIn, const std::vector<DataLine> & otherInputs);
};



//Tints the screen a certain color by multiplying the input color.
class ColorTintEffect : public PostProcessEffect
{
public:

    virtual std::string GetName(void) const override { return "colorTintEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override { assert(index == 0); return GetName() + std::to_string(GetUniqueID()) + "_tinted"; }

    ColorTintEffect(DataLine colorScales = DataLine(VectorF(1.0f, 1.0f, 1.0f)),
                    DataLine colorDepthIn = ColorDepthSamplerIn())
        : PostProcessEffect(colorDepthIn, DataNode::MakeVector(colorScales)) { }
    

protected:

    virtual void WriteMyOutputs(std::string & strOut) const override
    {
        //Tint the color, leave the depth unchanged.
        strOut += "\tvec4 " + GetOutputName(0) + " = vec4(" + GetInputs()[0].GetValue() + " * " + GetColorInput().GetValue() + ", " + GetDepthInput().GetValue() + ");\n";
    }
};



//Adds contrast to the screen using smoothstep or a custom "superSmoothstep" function.
class ContrastEffect : public PostProcessEffect
{
public:

    virtual std::string GetName(void) const override { return "contrastEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override { assert(index == 0); return GetName() + std::to_string(GetUniqueID()) + "_upContrast"; }


    //Different kinds of contrast amounts.
    enum Strengths
    {
        S_Light,
        S_Heavy,
    };
    
    //Stronger contrast generally means a larger performance overhead.
    Strengths Strength;
    //More iterations results in a stronger contrast but also more performance overhead.
    unsigned int Iterations;

    ContrastEffect(Strengths strength, unsigned int iterations,
                   DataLine colorDepthIn = ColorDepthSamplerIn())
        : PostProcessEffect(colorDepthIn, std::vector<DataLine>()), Strength(strength), Iterations(iterations) { }


protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;

    virtual void WriteMyOutputs(std::string & strOut) const override;
};



class FogEffect : public PostProcessEffect
{
public:
    
    virtual std::string GetName(void) const override { return "fogEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override { assert(index == 0); return GetName() + std::to_string(GetUniqueID()) + "_foggy"; }


    float Dropoff;
    Vector3f FogColor;

    FogEffect(DataLine colorDepthIn = ColorDepthSamplerIn(), float dropoff = 1.0f, Vector3f fogColor = Vector3f(1.0f, 1.0f, 1.0f))
        : PostProcessEffect(colorDepthIn, std::vector<DataLine>()),
          Dropoff(dropoff), FogColor(fogColor)
    {
    }


protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override
    {
        outDecls.insert(outDecls.end(),
"vec3 blendWithFog" + std::to_string(GetUniqueID()) + "(vec3 colorIn, float depthIn)\n\
{                                                          \n\
    float fogLerp = pow(depthIn, " + DataLine(VectorF(Dropoff)).GetValue() + ");                 \n\
    return mix(colorIn, " +
               DataLine(VectorF(FogColor)).GetValue() + ", " +
               "depthIn);                                  \n\
                                                           \n\
}                                                          \n\
");
    }

    virtual void WriteMyOutputs(std::string & strOut) const override
    {
        strOut += "\tvec4 " + GetOutputName(0) + " = vec4(blendWithFog(" + GetColorInput().GetValue() + ", " +
                                                                            GetDepthInput().GetValue() + "), " +
                                                          GetDepthInput().GetValue() + ");\n";
    }
};