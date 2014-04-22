#pragma once

#include "../DataNode.h"
#include "../../MaterialData.h"


//Represents the value of a vertex output.
//NOTE: This node is only usable in the fragment shader!
//Since vertex outputs can be any size, the output size is specified in this class's constructor.
class VertexOutputNode : public DataNode
{
public:


    virtual std::string GetName(void) const override { return "vertexOutputNode"; }
    virtual std::string GetOutputName(unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + std::to_string(outputIndex));
        Assert(GetShaderType() == Shaders::SH_Fragment_Shader, std::string() + "Invalid shader type (must be Fragment): " + std::to_string(GetShaderType()));

        return MaterialConstants::VertexOutNameBase + std::to_string(GetVertexOutputNumber(vertOutput));
    }

    VertexOutputNode(RenderingChannels vertexOutput, unsigned int outputSize)
        : vertOutput(vertexOutput), DataNode(std::vector<DataLine>(), MakeVector(outputSize))
    {
        Assert(IsChannelVertexOutput(vertexOutput, false),
               std::string() + "The vertex output argument isn't a vertex output! It is " + std::to_string(vertexOutput));
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        Assert(GetShaderType() == Shaders::SH_Fragment_Shader,
               std::string() + "Invalid shader type (must be Fragment): " + std::to_string(GetShaderType()));
        //Don't do anything, since the output name is an "in" variable.
    }
    
private:

    RenderingChannels vertOutput;
};