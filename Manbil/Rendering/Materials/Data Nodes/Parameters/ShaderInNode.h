#pragma once

#include "../DataNode.h"


//Handles an identical shader input across different kinds of shaders.
class ShaderInNode : public DataNode
{
public:

    bool HasVertexInput(void) const { return vInputIndex >= 0; }
    bool HasGeometryInput(void) const { return gInputIndex >= 0; }
    bool HasFragmentInput(void) const { return fInputIndex >= 0; }


    virtual std::string GetTypeName(void) const override { return "shaderInputs"; }


    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int outputIndex) const override;


    ShaderInNode(unsigned int size, std::string name = "", int vertexInputIndex = -1, int fragmentInputIndex = -1,
                 int geometryInputIndex = -1, unsigned int geometryInputArrayIndex = 0);

protected:

    virtual void WriteMyOutputs(std::string & outCode) const;


private:

    int vInputIndex, gInputIndex, fInputIndex;
    unsigned int gInputArrayIndex;

    unsigned int outSize;
};