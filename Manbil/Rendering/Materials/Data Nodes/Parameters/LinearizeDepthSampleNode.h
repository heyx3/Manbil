#pragma once

#include "ProjectionDataNode.h"


//Linearizes the value of a depth texture sample.
//Allows for easy manipulation, e.x. for fog.
class LinearizeDepthSampleNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    LinearizeDepthSampleNode(const DataLine & depthSample, std::string name = "");


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override;
    virtual void WriteMyOutputs(std::string & outStr) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void AssertMyInputsValid(void) const override;


    MAKE_NODE_READABLE_H(LinearizeDepthSampleNode)
};