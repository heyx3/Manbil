#pragma once

#include "../DataNode.h"
#include "../../../Texture Management/TextureChannels.h"


//Outputs the result of sampling a 3D texture.
class TextureSample3DNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "textureSample3DNode"; }
    std::string GetSamplerUniformName(void) const { return samplerName; }


    TextureSample3DNode(const DataLine & UVs, std::string _samplerName = "");

    virtual std::string GetOutputName(unsigned int index) const override;

    static unsigned int GetOutputIndex(ChannelsOut channel);


protected:

    virtual void GetMyParameterDeclarations(UniformDictionary & uniforms) const override;

    virtual void WriteMyOutputs(std::string & outCode) const override;

private:

    static std::vector<unsigned int> makeVector(void);

    std::string GetSampleOutputName(void) const { return GetName() + ToString(GetUniqueID()) + samplerName; }

    const DataLine & GetUVInput(void) const { return GetInputs()[0]; }

    //Gets the size of the output VectorF for the given sampling channel.
    static unsigned int GetSize(ChannelsOut channel);


    std::string samplerName;
};