#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"


//Outputs the result of sampling a texture.
class TextureSampleNode : public DataNode
{
public:

    ChannelsOut GetChannel(void) const { return channel; }

    virtual std::string GetName(void) const override { return "textureSampleNode"; }


    TextureSampleNode(ChannelsOut channel, const DataLine & UVs,
                      DataLine uvScale = DataLine(Vector(1.0f)),
                      DataLine uvPan = DataLine(Vector(Vector2f(0.0f, 0.0f))),
                      DataLine uvOffset = DataLine(Vector(Vector2f(0.0f, 0.0f))))
    : channel(channel), DataNode(MakeVector(UVs, uvScale, uvPan, uvOffset), MakeVector(GetSize(channel))) { }


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms, Shaders shaderType) const override
    {
        uniforms.TextureUniforms[GetSamplerUniformName()] = UniformSamplerValue(SFTexPtr(0), 0, GetSamplerUniformName());
    }

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;

private:

    const DataLine & GetUVInput(void) const { return GetInputs()[0]; }
    const DataLine & GetUVScaleInput(void) const { return GetInputs()[1]; }
    const DataLine & GetUVPanInput(void) const { return GetInputs()[2]; }
    const DataLine & GetUVOffsetInput(void) const { return GetInputs()[3]; }

    ChannelsOut channel;

    //Gets the size of the output Vector for the given sampling channel.
    static unsigned int GetSize(ChannelsOut channel)
    {
        switch (channel)
        {
            case ChannelsOut::Red:
            case ChannelsOut::Green:
            case ChannelsOut::Blue:
            case ChannelsOut::Alpha:
                return 1;
            case ChannelsOut::AllColorChannels:
                return 3;
            case ChannelsOut::AllChannels:
                return 4;

            default: assert(false);
        }
    }

    std::string GetSamplerUniformName(void) const { return GetName() + std::to_string(GetUniqueID()) + "_" + "sampler"; }
};