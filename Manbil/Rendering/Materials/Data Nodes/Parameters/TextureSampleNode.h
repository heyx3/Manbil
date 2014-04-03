#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"
#include "TimeNode.h"
#include "ShaderInNodes.h"


//Outputs the result of sampling a texture.
//TODO: Move most of these into the .cpp file.
//TODO: Remove scaling, panning, and offset -- just let the user do those things manually through the data node system.
class TextureSampleNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "textureSampleNode"; }
    std::string GetSamplerUniformName(void) const { return samplerName; }


    TextureSampleNode(std::string _samplerName = "",
                      DataLine UVs = DataLine(DataNodePtr(new UVNode()), 0),
                      DataLine uvScale = DataLine(VectorF(1.0f)),
                      DataLine uvPan = DataLine(VectorF(Vector2f(0.0f, 0.0f))),
                      DataLine uvOffset = DataLine(VectorF(Vector2f(0.0f, 0.0f))),
                      DataLine timeInput = DataLine(DataNodePtr(new TimeNode()), 0))
    : DataNode(makeVector(UVs, uvScale, uvPan, uvOffset, timeInput), makeVector())
    {
        assert(UVs.GetDataLineSize() == 2 && uvScale.GetDataLineSize() <= 2 &&
               uvPan.GetDataLineSize() == 2 && uvOffset.GetDataLineSize() == 2 &&
               timeInput.GetDataLineSize() == 1);

        samplerName = _samplerName;
        if (samplerName.empty())
            samplerName = GetName() + std::to_string(GetUniqueID()) + "_" + "sample";
    }

    virtual std::string GetOutputName(unsigned int index) const override
    {
        std::string base = GetSampleOutputName();

        switch (index)
        {
            case 0: return base + ".x";
            case 1: return base + ".y";
            case 2: return base + ".z";
            case 3: return base + ".w";
            case 4: return base + ".xyz";
            case 5: return base;
            default: assert(false);
        }
    }

    static unsigned int GetOutputIndex(ChannelsOut channel)
    {
        switch (channel)
        {
            case ChannelsOut::CO_Red: return 0;
            case ChannelsOut::CO_Green: return 1;
            case ChannelsOut::CO_Blue: return 2;
            case ChannelsOut::CO_Alpha: return 3;
            case ChannelsOut::CO_AllColorChannels: return 4;
            case ChannelsOut::CO_AllChannels: return 5;

            default: assert(false);
        }
    }


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms) const override
    {
        uniforms.TextureUniforms[GetSamplerUniformName()] = UniformSamplerValue(GetSamplerUniformName());
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

    std::string GetSampleOutputName(void) const { return samplerName + "_sample" + std::to_string(GetUniqueID()); }

    const DataLine & GetUVInput(void) const { return GetInputs()[0]; }
    const DataLine & GetUVScaleInput(void) const { return GetInputs()[1]; }
    const DataLine & GetUVPanInput(void) const { return GetInputs()[2]; }
    const DataLine & GetUVOffsetInput(void) const { return GetInputs()[3]; }
    const DataLine & GetTimeInput(void) const { return GetInputs()[4]; }

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