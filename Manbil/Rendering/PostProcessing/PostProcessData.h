#pragma once

/*

#include <string>
#include "../Materials/Data Nodes/DataNodes.hpp"


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
    DataLine ColorSamplerIn(std::string namePrefix, int fragmentUVInputIndex = 0)
    {
        DataLine uv(FragmentInputNode::GetInstanceName(), fragmentUVInputIndex);
        
        colorSamplerPtr = Ptr(0);
        colorSamplerPtr = Ptr(new TextureSample2DNode(uv, ColorSampler, namePrefix + "ColorSampler"));

        return DataLine(colorSamplerPtr->GetName(),
                        TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
    }
    //Returns a DataLine that samples and linearizes the depth texture.
    DataLine DepthSamplerIn(std::string namePrefix, unsigned int fragmentUVInputIndex = 0)
    {
        DataLine uv(FragmentInputNode::GetInstanceName(), fragmentUVInputIndex);

        depthSamplerPtr = Ptr(0);
        depthSamplerPtr = Ptr(new TextureSample2DNode(uv, DepthSampler, namePrefix + "DepthSampler"));

        depthLinearizerPtr = Ptr(0);
        depthLinearizerPtr =
            Ptr(new LinearizeDepthSampleNode(DataLine(depthSamplerPtr->GetName(),
                                                      TextureSample2DNode::GetOutputIndex(CO_Red))));

        return DataLine(depthLinearizerPtr->GetName());
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


    PostProcessEffect(PpePtr previousEffect = PpePtr(), std::vector<DataLine> otherInputs = std::vector<DataLine>(),
                      unsigned int numbPasses = 1, std::string name = "")
        : DataNode(MakeVector(previousEffect,
                              (name.empty() ? ("node" + ToString(GenerateUniqueID()) + "_") : (name + "_")),
                              otherInputs),
                   name),
          PrevEffect(previousEffect), NumbPasses(numbPasses), CurrentPass(1)
    {
        ReplaceInput(GetInputs().size() - 2, ColorSamplerIn(GetName() + "_"));
        ReplaceInput(GetInputs().size() - 1, DepthSamplerIn(GetName() + "_"));
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
    
    DataNode::Ptr colorSamplerPtr, depthSamplerPtr, depthLinearizerPtr;

    //Gets all inputs in the correct order not including the color and depth input.
    std::vector<DataLine> GetNonColorDepthInputs(void) const { return std::vector<DataLine>(GetInputs().begin(), GetInputs().end() - 2); }

    virtual void AssertMyInputsValid(void) const override
    {
        Assert(GetColorInput().GetSize() == 3,
               "Invalid color input size (must be 3): " + ToString(GetColorInput().GetSize()));
        Assert(GetDepthInput().GetSize() == 1,
               "Invalid depth input size (must be 1): " + ToString(GetDepthInput().GetSize()));
    }


private:

    //Creates the input std::vector for this node (by appending the correct color and depth inputs to the given vector).
    std::vector<DataLine> MakeVector(PpePtr prevEffect, std::string namePrefix, const std::vector<DataLine> & otherInputs);
};



//Tints the screen a certain color by multiplying the input color.
class ColorTintEffect : public PostProcessEffect
{
public:

    ColorTintEffect(DataLine colorScales = DataLine(VectorF(1.0f, 1.0f, 1.0f)), std::string name = "", PpePtr previousEffect = PpePtr())
        : PostProcessEffect(previousEffect, DataNode::MakeVector(colorScales))
    { }
    
protected:

    virtual void WriteMyOutputs(std::string & strOut) const override
    {
        //Tint the color.
        strOut += "\tvec3 " + GetOutputName(0) + " = " + GetInputs()[0].GetValue() + " * " + GetColorInput().GetValue() + ";\n";
    }
    
    virtual void AssertMyInputsValid(void) const override
    {
        PostProcessEffect::AssertMyInputsValid();
        Assert(GetInputs()[0].GetSize() == 3,
               "Invalid color scale size (must be 3): " + ToString(GetInputs()[0].GetSize()));
    }


    ADD_NODE_REFLECTION_DATA_H(ColorTintEffect)
};



//Adds contrast to the screen using smoothstep or a custom "superSmoothstep" function.
class ContrastEffect : public PostProcessEffect
{
public:


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
        : PostProcessEffect(prevEffect, std::vector<DataLine>(), 1, name),
          Strength(strength), Iterations(iterations) { }


protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;

    virtual void WriteMyOutputs(std::string & strOut) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;

    
    ADD_NODE_REFLECTION_DATA_H(ContrastEffect)
};




class FogEffect : public PostProcessEffect
{
public:

    //TODO: More parameters (e.x. fog start distance), and optimize so that parameters that are set to default values aren't actually used in computation.

    FogEffect(DataLine dropoff = DataLine(VectorF(1.0f)),
              DataLine fogColor = DataLine(VectorF(Vector3f(1.0f, 1.0f, 1.0f))),
              DataLine fogThickness = DataLine(VectorF(1.0f)),
              std::string name = "",
              PpePtr prevEffect = PpePtr())
        : PostProcessEffect(prevEffect, DataNode::MakeVector(dropoff, fogColor, fogThickness), 1, name) { }


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override;

    virtual void AssertMyInputsValid(void) const override
    {
        PostProcessEffect::AssertMyInputsValid();
        Assert(GetInputs()[0].GetSize() == 1,
               "Invalid dropoff size (must be 1): " + ToString(GetInputs()[0].GetSize()));
        Assert(GetInputs()[1].GetSize() == 3,
               "Invalid fog color size (must be 3): " + ToString(GetInputs()[1].GetSize()));
        Assert(GetInputs()[2].GetSize() == 1,
               "Invalid fog thickness size (must be 1): " + ToString(GetInputs()[2].GetSize()));
    }


private:

    const DataLine & GetDropoffInput(void) const { return GetInputs()[0]; }
    const DataLine & GetFogColorInput(void) const { return GetInputs()[1]; }
    const DataLine & GetFogThicknessInput(void) const { return GetInputs()[2]; }


    ADD_NODE_REFLECTION_DATA_H(FogEffect)
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

    GaussianBlurEffect(std::string name = "", PpePtr prevEffect = PpePtr())
        : PostProcessEffect(prevEffect, std::vector<DataLine>(), 4, name) { }

    virtual void OverrideVertexOutputs(std::vector<ShaderOutput> & vertOuts) const override;


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override;


private:

    mutable Ptr uvAddPtrs[15];

    ADD_NODE_REFLECTION_DATA_H(GaussianBlurEffect)
};

*/