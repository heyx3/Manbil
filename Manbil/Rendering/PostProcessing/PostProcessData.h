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
class PostProcessEffect : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "unknownPostProcessEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        assert(index < GetOutputs().size());
        return GetName() + std::to_string(GetUniqueID()) + "_UNKNOWN" + std::to_string(index);
    }

    //The names of the color/depth texture sampler uniforms.
    static const std::string ColorSampler, DepthSampler;


    //The number of passes needed to do this effect.
    unsigned int NumbPasses;
    //The current pass. Used when generating GLSL code.
    unsigned int CurrentPass;

    PostProcessEffect(DataLine colorIn, DataLine depthIn, std::vector<DataLine> otherInputs, unsigned int numbPasses = 1)
        : DataNode(MakeVector(colorIn, depthIn, otherInputs), DataNode::MakeVector(3, 1)),
          NumbPasses(numbPasses), CurrentPass(0)
    {
        assert(GetColorInput().GetDataLineSize() == 3);
        assert(GetDepthInput().GetDataLineSize() == 1);
    }

    //Creates a copy of this effect but with the default texture sampler as the color/depth input.
    virtual std::unique_ptr<DataNode> ReplaceWithDefaultInput(void) const = 0;

    //Returns a DataLine that samples the color texture.
    //This should be the value for the first post-process effect's "colorIn" constructor argument.
    //Subsequent effects should use the previous effect's color output.
    static DataLine ColorSamplerIn(void)
    {
        return DataLine(DataNodePtr(new TextureSampleNode(ColorSampler)),
                        TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));
    }
    //Returns a DataLine that samples the depth texture.
    //This should be the value for the first post-process effect's "depthIn" constructor argument.
    //Subsequent effects should use the previous effect's depth output.
    static DataLine DepthSamplerIn(void)
    {
        //TODO: Make DataNodes that get the max/min z distance from the camera (using built-in uniforms in Material). Remap depth from 0 to 1 using the equation from http://stackoverflow.com/questions/6652253/getting-the-true-z-value-from-the-depth-buffer
        DataLine depthTex(DataNodePtr(new TextureSampleNode(DepthSampler)),
                          TextureSampleNode::GetOutputIndex(ChannelsOut::CO_Red));
        DataLine linearDepth(DataNodePtr(new LinearDepthSampleNode(depthTex)), 0);
        return linearDepth;
    }

protected:

    //Gets the color input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetColorInput(void) const { return GetInputs()[GetInputs().size() - 2]; }
    //Gets the depth input (either the framebuffer or the previous post-process effect's output).
    const DataLine & GetDepthInput(void) const { return GetInputs()[GetInputs().size() - 1]; }

    //Gets all inputs in the correct order not including the color and depth input.
    std::vector<DataLine> GetNonColorDepthInputs(void) const
    {
        std::vector<DataLine> inputs;
        for (unsigned int i = 0; i < GetInputs().size() - 2; ++i)
            inputs.insert(inputs.end(), GetInputs()[i]);
        return inputs;
    }

private:

    //Creates the input std::vector for this node (by appending the color and depth inputs to the given vector).
    static std::vector<DataLine> MakeVector(DataLine colorIn, DataLine depthIn, const std::vector<DataLine> & otherInputs)
    {
        std::vector<DataLine> ret = otherInputs;
        ret.insert(ret.end(), colorIn);
        ret.insert(ret.end(), depthIn);
        return ret;
    }
};



//Tints the screen a certain color by multiplying the input color.
class ColorTintEffect : public PostProcessEffect
{
public:

    virtual std::string GetName(void) const override { return "colorTintEffect"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        assert(index <= 1);
        return GetName() + std::to_string(GetUniqueID()) +
               (index == 0 ? "_tinted" : "_unchangedDepth");
    }

    ColorTintEffect(DataLine colorScales = DataLine(VectorF(1.0f, 1.0f, 1.0f)),
                    DataLine colorIn = ColorSamplerIn(),
                    DataLine depthIn = DepthSamplerIn())
        : PostProcessEffect(colorIn, depthIn, DataNode::MakeVector(colorScales)) { }
    
    virtual std::unique_ptr<DataNode> ReplaceWithDefaultInput(void) const
    {
        return std::unique_ptr<DataNode>(new ColorTintEffect(GetInputs()[0]));
    }


protected:

    virtual void WriteMyOutputs(std::string & strOut) const override
    {
        //Tint the color, leave the depth unchanged.
        strOut += "\tvec3 " + GetOutputName(0) + " = " + GetInputs()[0].GetValue() + " * " + GetColorInput().GetValue() + ";\n";
        strOut += "\tfloat " + GetOutputName(1) + " = " + GetDepthInput().GetValue() + ";\n";
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
        return GetName() + std::to_string(GetUniqueID()) +
            (index == 0 ? "_upContrast" : "_unchangedDepth");
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
                   DataLine colorIn = ColorSamplerIn(),
                   DataLine depthIn = DepthSamplerIn())
        : PostProcessEffect(colorIn, depthIn, std::vector<DataLine>()), Strength(strength), Iterations(iterations) { }

    virtual std::unique_ptr<DataNode> ReplaceWithDefaultInput(void) const
    {
        return std::unique_ptr<DataNode>(new ContrastEffect(Strength, Iterations));
    }

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
        return GetName() + std::to_string(GetUniqueID()) +
            (index == 0 ? "_foggy" : "_unchangedDepth");
    }


    float Dropoff;
    Vector3f FogColor;
    //TODO: More parameters (max fog thickness, start distance, etc), and optimize so that parameters that are set to default values aren't actually used in computation.

    FogEffect(float dropoff = 1.0f, Vector3f fogColor = Vector3f(1.0f, 1.0f, 1.0f),
              DataLine colorIn = ColorSamplerIn(),
              DataLine depthIn = DepthSamplerIn())
        : PostProcessEffect(colorIn, depthIn, std::vector<DataLine>()), Dropoff(dropoff), FogColor(fogColor)
    {
    }

    virtual std::unique_ptr<DataNode> ReplaceWithDefaultInput(void) const
    {
        return std::unique_ptr<DataNode>(new FogEffect(Dropoff, FogColor));
    }

protected:

    virtual void GetMyFunctionDeclarations(std::vector<std::string> & outDecls) const override;

    virtual void WriteMyOutputs(std::string & strOut) const override;
};


//TODO: Parameterized fog -- identical to FogEffect but uses DataLines for dropoff and color instead of constants.