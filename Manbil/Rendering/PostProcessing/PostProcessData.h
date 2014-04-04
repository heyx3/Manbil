#pragma once

#include <string>
#include "../Materials/Data Nodes/DataNodeIncludes.h"



//Takes in a depth texture sample (in other words, the depth buffer value from 0 to 1)
//      and linearizes it based on the camera's zFar and zNear.
class LinearDepthSampleNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "linearDepthSampleNode"; }
    virtual std::string GetOutputName(unsigned int index) const override { assert(index == 0); return GetName() + std::to_string(GetUniqueID()) + "_linearized"; }

    LinearDepthSampleNode(const DataLine & depthSampleInput) : DataNode(MakeVector(depthSampleInput), MakeVector(1)) { }


protected:

    virtual void WriteMyOutputs(std::string & outStr) const override
    {
        std::string zn = MaterialConstants::CameraZNearName,
                    zf = MaterialConstants::CameraZFarName;
        outStr += "\tfloat " + GetOutputName(0) + " = (2.0 * " + zn + ") / (" + zf + " + " + zn + " - (" +
                                                                      GetInputs()[0].GetValue() + " * (" + zf + " - " + zn + ")));\n";
    }
};



//Abstract class representing a post-process effect as a DataNode.
//Outputs 0: the color output as a vec3, 1: the linear depth output as a float from 0 to 1.
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
    static DataLine ColorSamplerIn(void)
    {
        return DataLine(DataNodePtr(new TextureSampleNode(ColorSampler)),
                        TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    }
    //Returns a DataLine that samples the depth texture.
    static DataLine DepthSamplerIn(void)
    {
        DataLine depthTex(DataNodePtr(new TextureSampleNode(DepthSampler)),
                          TextureSampleNode::GetOutputIndex(ChannelsOut::CO_Red));
        DataLine linearDepth(DataNodePtr(new LinearDepthSampleNode(depthTex)), 0);
        return linearDepth;
    }


    //The effect that came before this one.
    PpePtr GetPreviousEffect(void) const { return PrevEffect; }
    //Switches out the effect this effect builds off of.
    void ChangePreviousEffect(PpePtr newPrevEffect = PpePtr())
    {
        PrevEffect = newPrevEffect;

        if (newPrevEffect.get() == 0)
            ReplaceInput(GetInputs().size() - 2, ColorSamplerIn());
        else ReplaceInput(GetInputs().size() - 2, DataLine(newPrevEffect, GetColorOutputIndex()));
    }

    //The number of passes needed to do this effect.
    unsigned int NumbPasses;
    //The current pass, starting at 1. Used when generating GLSL code.
    unsigned int CurrentPass;

    //Default name for a post-processing effect.
    virtual std::string GetName(void) const override { return "unknownPostProcessEffect"; }
    //Gets the depth output name only.
    virtual std::string GetOutputName(unsigned int index) const override
    {
        assert(index == 1);
        if (PrevEffect.get() == 0)
            return GetDepthInput().GetValue();
        else return PrevEffect->GetOutputName(index);
    }


    PostProcessEffect(PpePtr previousEffect = PpePtr(), std::vector<DataLine> otherInputs = std::vector<DataLine>(), unsigned int numbPasses = 1)
        : DataNode(MakeVector(previousEffect, otherInputs), DataNode::MakeVector(3, 1)),
          PrevEffect(previousEffect), NumbPasses(numbPasses), CurrentPass(1)
    {
        assert(GetColorInput().GetDataLineSize() == 3);
        assert(GetDepthInput().GetDataLineSize() == 1);
    }


    //Gets the color input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetColorInput(void) const { return GetInputs()[GetInputs().size() - 2]; }
    //Gets the depth input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetDepthInput(void) const { return GetInputs()[GetInputs().size() - 1]; }


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


private:

    //Creates the input std::vector for this node (by appending the correct color and depth inputs to the given vector).
    static std::vector<DataLine> MakeVector(PpePtr prevEffect, const std::vector<DataLine> & otherInputs)
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
};



//Tints the screen a certain color by multiplying the input color.
class ColorTintEffect : public PostProcessEffect
{
public:

    virtual std::string GetName(void) const override { return "tintEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        assert(index <= 1);
        return (index == 0 ?
                  (GetName() + std::to_string(GetUniqueID()) + "_tinted") :
                   PostProcessEffect::GetOutputName(index));
    }

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
    virtual std::string GetOutputName(unsigned int index) const override
    {
        assert(index <= 1);
        return (index == 0 ?
                (GetName() + std::to_string(GetUniqueID()) + "_upContrast") :
                PostProcessEffect::GetOutputName(index));
    }


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
    virtual std::string GetOutputName(unsigned int index) const override
    {
        assert(index <= 1);
        return (index == 0 ?
                (GetName() + std::to_string(GetUniqueID()) + "_foggy") :
                PostProcessEffect::GetOutputName(index));
    }


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




class TestMultiPassEffect : public PostProcessEffect
{
public:

    virtual std::string GetName(void) const override { return "titsEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        assert(index <= 1);
        return (index == 0 ?
                (GetName() + std::to_string(GetUniqueID()) + "_effect") :
                PostProcessEffect::GetOutputName(index));
    }

    TestMultiPassEffect(PpePtr prevEffect = PpePtr())
        : PostProcessEffect(prevEffect, std::vector<DataLine>(), 4) { }

protected:

    virtual void WriteMyOutputs(std::string & strOut) const override
    {
        assert(CurrentPass >= 1 && CurrentPass <= 4);

        strOut += "\tvec3 " + GetOutputName(0) + " = ";
        std::string col = GetColorInput().GetValue();

        switch (CurrentPass)
        {
        case 1:
            strOut += col + " * 0.5";
            break;
        case 2:
            strOut += col + " * 2.0";
            break;
        case 3:
            strOut += col + " * 0.5";
            break;
        case 4:
            strOut += col + " * 2.0";
            break;

        default: assert(false);
        }

        strOut += ";\n";
    }
};