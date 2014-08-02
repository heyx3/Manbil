#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"


//Outputs the result of sampling a cubemap texture.
class TextureSampleCubemapNode : public DataNode
{
public:

    //Gets the output index for a cubemap texture sample node that outputs the given data.
    static unsigned int GetOutputIndex(ChannelsOut channel);


    std::string SamplerName;

    TextureSampleCubemapNode(const DataLine & textureCoords, std::string _samplerName = "", std::string name = "");


    virtual std::string GetTypeName(void) const override { return "cubemapTextureSampler"; }

    virtual unsigned int GetNumbOutputs(void) const override { return 6; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;

    virtual std::string GetInputDescription(unsigned int index) const override;


private:

    std::string GetSampleOutputName(void) const { return GetName() + "_sampled"; }
};