#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"
#include "TimeNode.h"
#include "ShaderInNodes.h"


//Outputs the result of sampling a texture.
//TODO: Output a vec4, and expose a function "unsigned int GetOutputIndex(ChannelsOut outs)".
class TextureSampleNode : public DataNode
{
public:

    ChannelsOut GetChannel(void) const { return channel; }

    virtual std::string GetName(void) const override { return "textureSampleNode"; }
    std::string GetSamplerUniformName(void) const { return GetName() + std::to_string(GetUniqueID()) + "_" + "sampler"; }


    TextureSampleNode(ChannelsOut channel,
                      DataLine UVs = DataLine(DataNodePtr(new UVNode()), 0),
                      DataLine uvScale = DataLine(Vector(1.0f)),
                      DataLine uvPan = DataLine(Vector(Vector2f(0.0f, 0.0f))),
                      DataLine uvOffset = DataLine(Vector(Vector2f(0.0f, 0.0f))),
                      DataLine timeInput = DataLine(DataNodePtr(new TimeNode()), 0))
    : channel(channel), DataNode(makeVector(UVs, uvScale, uvPan, uvOffset, timeInput), MakeVector(GetSize(channel)))
    {
        assert(UVs.GetDataLineSize() == 2 && uvScale.GetDataLineSize() <= 2 &&
               uvPan.GetDataLineSize() == 2 && uvOffset.GetDataLineSize() == 2 &&
               timeInput.GetDataLineSize() == 1);
    }

    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms) const override
    {
        uniforms.TextureUniforms[GetSamplerUniformName()] = UniformSamplerValue(0, 0, GetSamplerUniformName());
    }

    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    static std::vector<DataLine> makeVector(const DataLine & uv, const DataLine & uvScale, const DataLine & uvPan,
                                            const DataLine & uvOffset, const DataLine & time)
    {
        std::vector<DataLine> dats = MakeVector(uv, uvScale, uvPan, uvOffset);
        dats.insert(dats.end(), time);
        return dats;
    }

    const DataLine & GetUVInput(void) const { return GetInputs()[0]; }
    const DataLine & GetUVScaleInput(void) const { return GetInputs()[1]; }
    const DataLine & GetUVPanInput(void) const { return GetInputs()[2]; }
    const DataLine & GetUVOffsetInput(void) const { return GetInputs()[3]; }
    const DataLine & GetTimeInput(void) const { return GetInputs()[4]; }

    ChannelsOut channel;

    //Gets the size of the output Vector for the given sampling channel.
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
};