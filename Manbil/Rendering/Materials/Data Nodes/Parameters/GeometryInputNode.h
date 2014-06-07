#pragma once

#include "../DataNode.h"
#include "../../MaterialData.h"


#pragma warning(disable: 4512 4100)

//Represents the value of vertex shader outputs into the geometry shader.
//NOTE: This node is only usable in the geometry shader!
//Since geometry inputs vary by material, the inputs are specified in the constructor.
//Each output index corresponds to that vertex output.
class GeometryInputNode : public DataNode
{
public:

    const VertexAttributes Attributes;

    virtual std::string GetName(void) const override { return "geometryInputNode"; }
    virtual std::string GetOutputName(unsigned int outputIndex) const override
    {
        Assert(GetShaderType() == Shaders::SH_GeometryShader,
               std::string() + "Invalid shader type (must be Geometry): " + ToString(GetShaderType()));

        Assert(outputIndex < Attributes.GetNumbAttributes(),
               std::string() + "Invalid input number " + std::to_string(outputIndex) +
               ", must be less than " + std::to_string(Attributes.GetNumbAttributes()));

        return MaterialConstants::VertexOutNameBase + std::to_string(outputIndex);
    }

    GeometryInputNode(const VertexAttributes & geoInAttributes)
        : DataNode(std::vector<DataLine>(), BuildVector(geoInAttributes)), Attributes(geoInAttributes)
    {
        unsigned int numb = geoInAttributes.GetNumbAttributes();
        for (unsigned int i = 0; i < numb; ++i)
        {
            Assert(geoInAttributes.GetAttributeSize(i) > 0, std::string() + "The geometry input size for index " + std::to_string(i) + " is 0! It must be between 1 and 4, inclusive.");
            Assert(geoInAttributes.GetAttributeSize(i) < 5, std::string() + "The geometry input size for index " + std::to_string(i) + " must be between 1 and 4, inclusive, but it is " + std::to_string(geoInAttributes.GetAttributeSize(i)));
        }
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        Assert(GetShaderType() == Shaders::SH_GeometryShader,
               std::string() + "Invalid shader type (must be Geometry): " + ToString(GetShaderType()));
        //Don't actually output anything, since the output name is an "in" variable.
    }


private:

    static std::vector<unsigned int> BuildVector(const VertexAttributes & gIAttributes)
    {
        std::vector<unsigned int> ret;
        unsigned int numb = gIAttributes.GetNumbAttributes();

        for (unsigned int i = 0; i < numb; ++i)
            ret.insert(ret.end(), gIAttributes.GetAttributeSize(i));

        return ret;
    }
};

#pragma warning(default: 4512 4100)