#pragma once

#include "../DataNode.h"
#include "../../MaterialData.h"
#include "../../../../Vertices.h"


//Represents the value of vertex inputs.
//NOTE: This node is only usable in the vertex shader!
//Since vertex inputs can be any size, this node's output size is specified in the class's constructor.
//Each output index corresponds to that vertex input.
class VertexInputNode : public DataNode
{
public:

    const VertexAttributes Attributes;

    virtual std::string GetName(void) const override { return "vertexInputNode"; }
    virtual std::string GetOutputName(unsigned int outputIndex) const override
    {
        Assert(outputIndex < Attributes.GetNumbAttributes(),
               std::string() + "Invalid attribute number " + std::to_string(outputIndex) +
                  ", must be less than " + std::to_string(Attributes.GetNumbAttributes()));
        Assert(GetShaderType() == Shaders::SH_Vertex_Shader,
               std::string() + "Invalid shader type (must be Vertex): " + ToString(GetShaderType()));

        return MaterialConstants::VertexInNameBase + std::to_string(outputIndex);
    }

    VertexInputNode(const VertexAttributes & attributes)
        : DataNode(std::vector<DataLine>(), BuildVector(attributes)), Attributes(attributes)
    {
        unsigned int numb = attributes.GetNumbAttributes();
        for (unsigned int i = 0; i < numb; ++i)
        {
            Assert(attributes.GetAttributeSize(i) > 0, std::string() + "The vertex input size for index " + std::to_string(i) + " is 0! It must be between 1 and 4, inclusive.");
            Assert(attributes.GetAttributeSize(i) < 5, std::string() + "The vertex input size for index " + std::to_string(i) + " must be between 1 and 4 inclusive, but it is " + std::to_string(attributes.GetAttributeSize(i)));
        }
    }

protected:

#pragma warning(disable: 4100)
    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        Assert(GetShaderType() == Shaders::SH_Vertex_Shader,
               std::string() + "Invalid shader type (must be Vertex): " + ToString(GetShaderType()));
        //Don't actually output anything, since the output name is an "in" variable.
    }
#pragma warning(default: 4100)

private:

    static std::vector<unsigned int> BuildVector(const VertexAttributes & attributes)
    {
        std::vector<unsigned int> ret;
        unsigned int numb = attributes.GetNumbAttributes();

        for (unsigned int i = 0; i < numb; ++i)
            ret.insert(ret.end(), attributes.GetAttributeSize(i));

        return ret;
    }
};