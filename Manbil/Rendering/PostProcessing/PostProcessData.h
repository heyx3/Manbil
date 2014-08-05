#pragma once

#include <string>
#include "../Materials/Data Nodes/DataNodeIncludes.h"


//Sets up post-process effects so that they can be read from a file.
extern void PreparePpeEffectsToBeRead(void);


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

    static std::vector<Ptr> * NodeStorage;


    static unsigned int GetColorOutputIndex(void) { return 0; }
    static unsigned int GetDepthOutputIndex(void) { return 1; }

    //Returns a DataLine that samples the color texture.
    //This should be the value for the first post-process effect's "colorIn" constructor argument.
    //Subsequent effects should use the previous effect's color output.
    static DataLine ColorSamplerIn(std::string namePrefix, int fragmentUVInputIndex = 0)
    {
        DataLine uv(FragmentInputNode::GetInstance()->GetName(), fragmentUVInputIndex);
        NodeStorage->insert(NodeStorage->end(), Ptr(new TextureSample2DNode(uv, ColorSampler, namePrefix + "ColorSampler")));
        return DataLine(namePrefix + "ColorSampler", TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    }
    //Returns a DataLine that samples and linearizes the depth texture.
    static DataLine DepthSamplerIn(std::string namePrefix, unsigned int fragmentUVInputIndex = 0)
    {
        DataLine uv(FragmentInputNode::GetInstance()->GetName(), fragmentUVInputIndex);
        NodeStorage->insert(NodeStorage->end(), Ptr(new TextureSample2DNode(uv, DepthSampler, namePrefix + "DepthSampler")));
        DataLine depthSample(namePrefix + "DepthSampler", TextureSample2DNode::GetOutputIndex(ChannelsOut::CO_Red));

        NodeStorage->insert(NodeStorage->end(), Ptr(new LinearizeDepthSampleNode(depthSample, namePrefix + "DepthLinearizer")));
        return DataLine(namePrefix + "DepthLinearizer", 0);
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

    //Gets the depth output size only.
    virtual unsigned int GetOutputSize(unsigned int index) const final;
    //Gets the depth output name only.
    virtual std::string GetOutputName(unsigned int index) const final;


    PostProcessEffect(NodeFactory nodeFactory, PpePtr previousEffect = PpePtr(), std::vector<DataLine> otherInputs = std::vector<DataLine>(),
                      unsigned int numbPasses = 1, std::string name = "")
        : DataNode(MakeVector(previousEffect, name + "_", otherInputs), nodeFactory, name),
          PrevEffect(previousEffect), NumbPasses(numbPasses), CurrentPass(1)
    {
        Assert(GetColorInput().GetSize() == 3, "Invalid color input size (must be 3): " + ToString(GetColorInput().GetSize()));
        Assert(GetDepthInput().GetSize() == 1, "Invalid depth input size (must be 1): " + ToString(GetDepthInput().GetSize()));
    }


    //Gets the color input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetColorInput(void) const { return GetInputs()[GetInputs().size() - 2]; }
    //Gets the depth input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetDepthInput(void) const { return GetInputs()[GetInputs().size() - 1]; }

    //Lets this effect set any vertex outputs it wants to (potentially overriding the default of UVs in slot 1).
    //NOTE: This could break if two effects try to override vertex outputs in the same pass!
    virtual void OverrideVertexOutputs(std::vector<ShaderOutput> & outs) const { }


protected:

    PpePtr PrevEffect;

    //Gets all inputs in the correct order not including the color and depth input.
    std::vector<DataLine> GetNonColorDepthInputs(void) const { return std::vector<DataLine>(GetInputs().begin(), GetInputs().end() - 2); }


private:

    //Creates the input std::vector for this node (by appending the correct color and depth inputs to the given vector).
    static std::vector<DataLine> MakeVector(PpePtr prevEffect, std::string namePrefix, const std::vector<DataLine> & otherInputs);
};



//Tints the screen a certain color by multiplying the input color.
class ColorTintEffect : public PostProcessEffect
{
public:

    virtual std::string GetTypeName(void) const override { return "Tint Effect"; }

    ColorTintEffect(DataLine colorScales = DataLine(VectorF(1.0f, 1.0f, 1.0f)), std::string name = "", PpePtr previousEffect = PpePtr())
        : PostProcessEffect([]() { return Ptr(new ColorTintEffect()); },
                            previousEffect, DataNode::MakeVector(colorScales))
    { }
    
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

    virtual std::string GetTypeName(void) const override { return "Contrast Effect"; }


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

    ContrastEffect(Strengths strength, unsigned int iterations, std::string name = "", PpePtr prevEffect = PpePtr())
        : PostProcessEffect([]() { return Ptr(new ContrastEffect(S_Light, 1)); },
                            prevEffect, std::vector<DataLine>(), 1, name),
          Strength(strength), Iterations(iterations) { }


protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;

    virtual void WriteMyOutputs(std::string & strOut) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;
};




class FogEffect : public PostProcessEffect
{
public:
    
    virtual std::string GetTypeName(void) const override { return "Fog Effect"; }

    //TODO: More parameters (e.x. fog start distance), and optimize so that parameters that are set to default values aren't actually used in computation.

    FogEffect(DataLine dropoff = DataLine(VectorF(1.0f)),
              DataLine fogColor = DataLine(VectorF(Vector3f(1.0f, 1.0f, 1.0f))),
              DataLine fogThickness = DataLine(VectorF(1.0f)),
              std::string name = "",
              PpePtr prevEffect = PpePtr())
        : PostProcessEffect([]() { return Ptr(new FogEffect()); }, prevEffect,
                            DataNode::MakeVector(dropoff, fogColor, fogThickness), 1, name) { }


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

    virtual std::string GetTypeName(void) const override { return "Gaussian Blur Effect"; }

    GaussianBlurEffect(std::string name = "", PpePtr prevEffect = PpePtr())
        : PostProcessEffect([]() { return Ptr(new GaussianBlurEffect()); }, prevEffect, std::vector<DataLine>(), 4, name) { }

    virtual void OverrideVertexOutputs(std::vector<ShaderOutput> & vertOuts) const override;


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override;
};