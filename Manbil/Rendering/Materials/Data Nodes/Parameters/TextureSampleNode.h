#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"


//Outputs the result of sampling a texture.
class TextureSampleNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "textureSampleNode"; }
    std::string GetSamplerUniformName(void) const { return samplerName; }


    TextureSampleNode(const DataLine & UVs, std::string _samplerName = "");

    virtual std::string GetOutputName(unsigned int index) const override;

    static unsigned int GetOutputIndex(ChannelsOut channel);


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms) const override
    {
        uniforms.TextureUniforms[GetSamplerUniformName()] = UniformSampler2DValue(GetSamplerUniformName());
    }

    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    static std::vector<unsigned int> makeVector(void)
    {
        std::vector<unsigned int> ints;
        ints.insert(ints.end(), 1);
        ints.insert(ints.end(), 1);
        ints.insert(ints.end(), 1);
        ints.insert(ints.end(), 1);
        ints.insert(ints.end(), 3);
        ints.insert(ints.end(), 4);
        return ints;
    }

    std::string GetSampleOutputName(void) const { return GetName() + ToString(GetUniqueID()) + samplerName; }

    const DataLine & GetUVInput(void) const { return GetInputs()[0]; }

    //Gets the size of the output VectorF for the given sampling channel.
    static unsigned int GetSize(ChannelsOut channel)
    {
        switch (channel)
        {
            case ChannelsOut::CO_Red:
            case ChannelsOut::CO_Green:
            case ChannelsOut::CO_Blue:
            case ChannelsOut::CO_Alpha:
                return 1;
            case ChannelsOut::CO_AllColorChannels:
                return 3;
            case ChannelsOut::CO_AllChannels:
                return 4;

            default: assert(false); return 0;
        }
    }


    std::string samplerName;
};