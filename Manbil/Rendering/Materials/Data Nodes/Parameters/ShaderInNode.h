#pragma once

#include "../DataNode.h"


//Handles an identical shader input across different kinds of shaders.
class ShaderInNode : public DataNode
{
public:

    bool HasVertexInput(void) const { return vInputIndex >= 0; }
    bool HasGeometryInput(void) const { return gInputIndex >= 0; }
    bool HasFragmentInput(void) const { return fInputIndex >= 0; }


    virtual std::string GetName(void) const override { return "shaderInNode"; }
    virtual std::string GetOutputName(unsigned int outputIndex) const override;


    ShaderInNode(unsigned int size, int vertexInputIndex = -1, int geometryInputIndex = -1, int fragmentInputIndex = -1)
        : vInputIndex(vertexInputIndex), gInputIndex(geometryInputIndex), fInputIndex(fragmentInputIndex),
          DataNode(std::vector<DataLine>(), MakeVector(size))
    {
        Assert(size > 0, "Size of input is 0! Must be from 1-4, inclusive.");
        Assert(size < 5, std::string() + "Size of input is " + std::to_string(size) + "! Must be from 1-4, inclusive.");
    }

protected:

    virtual void WriteMyOutputs(std::string & outCode) const
    {
        //Don't write anything; this node outputs shader inputs.
    }


private:

    int vInputIndex, gInputIndex, fInputIndex;
};