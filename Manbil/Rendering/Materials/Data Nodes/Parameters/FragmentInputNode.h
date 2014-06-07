#pragma once

#include <vector>
#include "../DataNode.h"
#include "../../MaterialData.h"
#include "../../../../Vertices.h"


#pragma warning(disable: 4512 4100)

//Represents the value of a fragment input (from the vertex shader, or the geometry shader if it exists).
//NOTE: This node is only usable in the fragment shader!
class FragmentInputNode : public DataNode
{
public:

    const VertexAttributes Attributes;

    virtual std::string GetName(void) const override { return "fragmentInputNode"; }
    virtual std::string GetOutputName(unsigned int outputIndex) const override
    {
        Assert(GetShaderType() == Shaders::SH_Fragment_Shader,
               std::string() + "Invalid shader type (must be Fragment): " + ToString(GetShaderType()));

        Assert(outputIndex < Attributes.GetNumbAttributes(),
               std::string() + "Invalid input number " + std::to_string(outputIndex) +
               ", must be less than " + std::to_string(Attributes.GetNumbAttributes()));

        if (GetGeoShaderData()->IsValidData())
        {
            return GetGeoShaderData()->OutputTypes.OutputNames[outputIndex];
        }
        else
        {
            return MaterialConstants::VertexOutNameBase + std::to_string(outputIndex);
        }
    }

    FragmentInputNode(const VertexAttributes & fragmentInputs)
        : DataNode(std::vector<DataLine>(), MakeVector(fragmentInputs)), Attributes(fragmentInputs)
    {
        unsigned int numb = fragmentInputs.GetNumbAttributes();
        for (unsigned int i = 0; i < numb; ++i)
        {
            Assert(fragmentInputs.GetAttributeSize(i) > 0, std::string() + "The fragment input size for index " + std::to_string(i) + " is 0! It must be between 1 and 4, inclusive.");
            Assert(fragmentInputs.GetAttributeSize(i) < 5, std::string() + "The fragment input size for index " + std::to_string(i) + " must be between 1 and 4, inclusive, but it is " + std::to_string(fragmentInputs.GetAttributeSize(i)));
        }
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        Assert(GetShaderType() == Shaders::SH_Fragment_Shader,
               std::string() + "Invalid shader type (must be Fragment): " + ToString(GetShaderType()));
        //Don't actually output anything, since the output name is an "in" variable.
    }


private:

    static std::vector<unsigned int> MakeVector(const VertexAttributes & inAtts)
    {
        std::vector<unsigned int> ret;

        unsigned int num = inAtts.GetNumbAttributes();
        for (unsigned int i = 0; i < num; ++i)
            ret.insert(ret.end(), inAtts.GetAttributeSize(i));

        return ret;
    }
};

#pragma warning(default: 4512 4100)