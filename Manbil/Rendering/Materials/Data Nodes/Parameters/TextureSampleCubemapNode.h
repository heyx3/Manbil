#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"


//Outputs the result of sampling a cubemap texture.
class TextureSampleCubemapNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "textureSampleCubemapNode"; }
    std::string GetSamplerUniformName(void) const { return samplerName; }


    TextureSampleCubemapNode(const DataLine & textureCoords, std::string _samplerName = "");

    virtual std::string GetOutputName(unsigned int index) const override;

    static unsigned int GetOutputIndex(ChannelsOut channel);


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms) const override;

    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    static std::vector<unsigned int> makeVector(void);

    std::string GetSampleOutputName(void) const { return GetName() + ToString(GetUniqueID()) + samplerName; }

    const DataLine & GetTexCoordInput(void) const { return GetInputs()[0]; }

    //Gets the size of the output VectorF for the given sampling channel.
    static unsigned int GetSize(ChannelsOut channel);


    std::string samplerName;
};