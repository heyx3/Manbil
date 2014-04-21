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
        assert(outputIndex == 0 && GetShaderType() == Shaders::SH_Fragment_Shader);
        return MaterialConstants::VertexOutNameBase + std::to_string(GetVertexOutputNumber(vertOutput));
    }

    VertexOutputNode(RenderingChannels vertexOutput, unsigned int outputSize)
        : vertOutput(vertexOutput), DataNode(std::vector<DataLine>(), MakeVector(outputSize))
    {
        assert(IsChannelVertexOutput(vertexOutput, false));
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        assert(GetShaderType() == Shaders::SH_Fragment_Shader);
        //Don't do anything, since the output name is an "in" variable.
    }
    
private:

    RenderingChannels vertOutput;
};