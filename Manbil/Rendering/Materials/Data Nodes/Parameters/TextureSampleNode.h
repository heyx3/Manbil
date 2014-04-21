#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"
#include "TimeNode.h"
#include "ShaderInNodes.h"


//Outputs the result of sampling a texture.
class TextureSampleNode : public DataNode
{
public:

    static DataNodePtr CreateComplexTexture(const DataLine & uvs, std::string samplerName = "",
                                            DataLine scale = DataLine(VectorF(Vector2f(1.0f, 1.0f))),
                                            DataLine pan = DataLine(VectorF(Vector2f(0.0f, 0.0f))),
                                            DataLine offset = DataLine(VectorF(Vector2f(0.0f, 0.0f))));


    virtual std::string GetName(void) const override { return "textureSampleNode"; }
    std::string GetSamplerUniformName(void) const { return samplerName; }


    TextureSampleNode(const DataLine & UVs, std::string _samplerName = "");

    virtual std::string GetOutputName(unsigned int index) const override;

    static unsigned int GetOutputIndex(ChannelsOut channel);


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms) const override
    {
        uniforms.TextureUniforms[GetSamplerUniformName()] = UniformSamplerValue(GetSamplerUniformName());
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

    std::string GetSampleOutputName(void) const { return samplerName; }

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

            default: assert(false);
        }
    }


    std::string samplerName;
};