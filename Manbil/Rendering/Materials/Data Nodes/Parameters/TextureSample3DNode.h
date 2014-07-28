#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"


//Outputs the result of sampling a 3D texture.
class TextureSample3DNode : public DataNode
{
public:

    //Gets the output index for a 3D texture sample node that outputs the given data.
    static unsigned int GetOutputIndex(ChannelsOut channel);


    std::string SamplerName;

    TextureSample3DNode(const DataLine & UVs, std::string _samplerName = "", std::string name = "");


    virtual std::string GetTypeName(void) const override { return "3DTextureSampler"; }

    virtual unsigned int GetNumbOutputs(void) const override { return 6; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual bool WriteExtraData(DataWriter * writer, std::string & outError) const override;
    virtual bool ReadExtraData(DataReader * reader, std::string & outError) override;


private:

    std::string GetSampleOutputName(void) const { return GetName() + "_sampled"; }
};