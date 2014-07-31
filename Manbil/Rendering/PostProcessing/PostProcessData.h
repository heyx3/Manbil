#pragma once

#include <string>
#include "../Materials/Data Nodes/DataNodeIncludes.h"
#include "../Helper Classes/DrawingQuad.h"


//Takes in a depth texture sample (in other words, the depth buffer value from 0 to 1)
//      and linearizes it based on the camera's zFar and zNear.
//TODO: Move out of "PostProcessData" and into one of the standard DataNode folders. Add to serialization system!
class LinearDepthSampleNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "depthLinearizerNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return GetName() + std::to_string(GetUniqueID()) + "_linearized";
    }

    LinearDepthSampleNode(const DataLine & depthSampleInput) : DataNode(BuildInputs(depthSampleInput), MakeVector(1)) { }


protected:

    virtual void WriteMyOutputs(std::string & outStr) const override
    {
        std::string zn = GetZNearInput().GetValue(),
                    zf = GetZFarInput().GetValue();
        outStr += "\tfloat " + GetOutputName(0) + " = (2.0 * " + zn + ") / " +
                                                     "(" + zf + " + " + zn + " - " +
                                                       "(" + GetDepthSampleInput().GetValue() + " * " +
                                                            "(" + zf + " - " + zn + ")));\n";
    }

private:

    static std::vector<DataLine> BuildInputs(const DataLine & depthSampler)
    {
        std::vector<DataLine> ret;
        ret.insert(ret.end(), ProjectionDataNode::GetZNear());
        ret.insert(ret.end(), ProjectionDataNode::GetZFar());
        ret.insert(ret.end(), depthSampler);
        return ret;
    }

    const DataLine & GetZNearInput(void) const { return GetInputs()[0]; }
    const DataLine & GetZFarInput(void) const { return GetInputs()[1]; }
    const DataLine & GetDepthSampleInput(void) const { return GetInputs()[2]; }
};



//Abstract class representing a post-process effect as a DataNode.
//Outputs:
//0: the color output as a vec3.
//1: the linear depth output as a float from 0 to 1.
//The depth output should never change after an effect is applied.
class PostProcessEffect : public DataNode
{
public:

    typedef std::shared_ptr<PostProcessEffect> PpePtr;


    //The names of the color/depth texture sampler uniforms.
    static const std::string ColorSampler, DepthSampler;

    static unsigned int GetColorOutputIndex(void) { return 0; }
    static unsigned int GetDepthOutputIndex(void) { return 1; }

    //Returns a DataLine that samples the color texture.
    //This should be the value for the first post-process effect's "colorIn" constructor argument.
    //Subsequent effects should use the previous effect's color output.
    static DataLine ColorSamplerIn(ShaderInOutAttributes fragmentIn = ShaderInOutAttributes(2, false), int fragmentUVInputIndex = 0)
    {
        return DataLine(DataNodePtr(new TextureSample2DNode(DataLine(DataNodePtr(new FragmentInputNode(fragmentIn)), fragmentUVInputIndex), ColorSampler)),
                        TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    }
    //Returns a DataLine that samples the depth texture.
    static DataLine DepthSamplerIn(ShaderInOutAttributes fragmentIn = ShaderInOutAttributes(2, false), int fragmentUVInputIndex = 0)
    {
        DataLine depthTex(DataNodePtr(new TextureSample2DNode(DataLine(DataNodePtr(new FragmentInputNode(fragmentIn)), fragmentUVInputIndex), DepthSampler)),
                          TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_Red));
        DataLine linearDepth(DataNodePtr(new LinearDepthSampleNode(depthTex)), 0);
        return linearDepth;
    }


    //TODO: Use this function to cut down on duplicate material passes.
    //Writes to the given std::vector the passes to perform, and the order to perform them in,
    //    to apply this effect the given number of times in succession.
    //Default behavior: just applies pass #1 once for each iteration.
    virtual void BuildPasses(unsigned int effectIterations, std::vector<unsigned int> & outPasses) const
    {
        for (unsigned int i = 0; i < effectIterations; ++i)
            outPasses.insert(outPasses.end(), 1);
    }


    //The effect that came before this one.
    PpePtr GetPreviousEffect(void) const { return PrevEffect; }
    //Switches out the effect this effect builds off of.
    virtual void ChangePreviousEffect(PpePtr newPrevEffect = PpePtr());

    //The number of passes needed to do this effect.
    unsigned int NumbPasses;
    //The current pass, starting at 1. Used when generating GLSL code.
    unsigned int CurrentPass;

    //Default name for a post-processing effect.
    virtual std::string GetName(void) const override { return "UNKNOWN_POST_PROCESS_EFFECT"; }
    //Gets the depth output name only.
    virtual std::string GetOutputName(unsigned int index) const override;


    PostProcessEffect(PpePtr previousEffect = PpePtr(), std::vector<DataLine> otherInputs = std::vector<DataLine>(), unsigned int numbPasses = 1)
        : DataNode(MakeVector(previousEffect, otherInputs), DataNode::MakeVector(3, 1)),
          PrevEffect(previousEffect), NumbPasses(numbPasses), CurrentPass(1)
    {
        Assert(GetColorInput().GetDataLineSize() == 3, std::string() + "Invalid color input size (must be 3): " + ToString(GetColorInput().GetDataLineSize()));
        Assert(GetDepthInput().GetDataLineSize() == 1, std::string() + "Invalid depth input size (must be 1): " + ToString(GetDepthInput().GetDataLineSize()));
    }


    //Gets the color input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetColorInput(void) const { return GetInputs()[GetInputs().size() - 2]; }
    //Gets the depth input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetDepthInput(void) const { return GetInputs()[GetInputs().size() - 1]; }

    //Lets this effect set any vertex outputs it wants to (potentially overriding the default of UVs in slot 1).
    //NOTE: This could break if two effects try to override vertex outputs in the same pass!
    virtual void OverrideVertexOutputs(std::unordered_map<RenderingChannels, DataLine> & channels) const { }


protected:

    PpePtr PrevEffect;

    //Gets all inputs in the correct order not including the color and depth input.
    std::vector<DataLine> GetNonColorDepthInputs(void) const
    {
        std::vector<DataLine> inputs;
        for (unsigned int i = 0; i < GetInputs().size() - 2; ++i)
            inputs.insert(inputs.end(), GetInputs()[i]);
        return inputs;
    }

    //Gets the inputs into the fragment shader. By default, the only input is UVs.
    virtual ShaderInOutAttributes GetFragmentInAttributes(void) const { return ShaderInOutAttributes(2, false); }
    //Gets the index of the fragment shader input that corresponds to UVs. By default, it is 0 (the first and only input).
    virtual int GetUVFragInputIndex(void) const { return 0; }


private:

    //Creates the input std::vector for this node (by appending the correct color and depth inputs to the given vector).
    static std::vector<DataLine> MakeVector(PpePtr prevEffect, const std::vector<DataLine> & otherInputs);
};



//Tints the screen a certain color by multiplying the input color.
class ColorTintEffect : public PostProcessEffect
{
public:

    virtual std::string GetName(void) const override { return "tintEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override;

    ColorTintEffect(DataLine colorScales = DataLine(VectorF(1.0f, 1.0f, 1.0f)),
                    PpePtr previousEffect = PpePtr())
        : PostProcessEffect(previousEffect, DataNode::MakeVector(colorScales)) { }
    
protected:

    virtual void WriteMyOutputs(std::string & strOut) const override
    {
        //Tint the color.
        strOut += "\tvec3 " + GetOutputName(0) + " = " + GetInputs()[0].GetValue() + " * " + GetColorInput().GetValue() + ";\n";
    }
};



//Adds contrast to the screen using smoothstep or a custom "superSmoothstep" function.
class ContrastEffect : public PostProcessEffect
{
public:

    virtual std::string GetName(void) const override { return "contrastEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override;


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
                   PpePtr prevEffect = PpePtr())
        : PostProcessEffect(prevEffect, std::vector<DataLine>()), Strength(strength), Iterations(iterations) { }

protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;

    virtual void WriteMyOutputs(std::string & strOut) const override;
};




class FogEffect : public PostProcessEffect
{
public:
    
    virtual std::string GetName(void) const override { return "fogEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override;


    //TODO: More parameters (e.x. fog start distance), and optimize so that parameters that are set to default values aren't actually used in computation.

    FogEffect(DataLine dropoff = DataLine(VectorF(1.0f)),
              DataLine fogColor = DataLine(VectorF(Vector3f(1.0f, 1.0f, 1.0f))),
              DataLine fogThickness = DataLine(VectorF(1.0f)),
              PpePtr prevEffect = PpePtr())
        : PostProcessEffect(prevEffect, DataNode::MakeVector(dropoff, fogColor, fogThickness)) { }

protected:

    virtual void WriteMyOutputs(std::string & strOut) const override;

private:

    const DataLine & GetDropoffInput(void) const { return GetInputs()[0]; }
    const DataLine & GetFogColorInput(void) const { return GetInputs()[1]; }
    const DataLine & GetFogThicknessInput(void) const { return GetInputs()[2]; }
};



//Applies a Gaussian blur to the world render.
//Gaussian blurs are computationally complex, so some tricky things are done to make this effect faster.
//First, it says that it has 4 passes, but the first and last passes don't
//   actually do anything; they just exist because the real passes need
//   to be in their own group, separate from any other effects.
//Second, it uses 15 vertex outputs to take advantage of GPU hardware instead of manually computing UV offsets.
class GaussianBlurEffect : public PostProcessEffect
{
public:

    virtual std::string GetName(void) const override { return "gaussBlurEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override;

    GaussianBlurEffect(PpePtr prevEffect = PpePtr()) : PostProcessEffect(prevEffect, std::vector<DataLine>(), 4) { }

    virtual void OverrideVertexOutputs(std::unordered_map<RenderingChannels, DataLine> & channels) const override;


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override;
};