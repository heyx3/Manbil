#pragma once

#include "../Rendering/Data Nodes/DataNode.h"
#include "../Rendering/Data Nodes/Parameters/ProjectionDataNode.h"


//A special DataNode that performs some constant number of randomized tests
//    of a given depth against a depth texture at around the given spot.
//Outputs a value between 0 and 1 indicating how often the test passed.
class MultiDepthTestNode : public DataNode
{
public:

    MultiDepthTestNode(std::string _samplerName, DataLine depthTexUV, DataLine testDepth,
                       std::string name = "");


    virtual unsigned int GetOutputSize(unsigned int outputIndex) const override { return 1; }
    virtual std::string GetOutputName(unsigned int outputIndex) const override;

    
protected:

    virtual void AssertMyInputsValid(void) const override;

    virtual void GetMyParameterDeclarations(UniformList& outUniforms) const override;
    virtual void WriteMyOutputs(std::string& outCode) const override;

    virtual void WriteExtraData(DataWriter* writer) const override;
    virtual void ReadExtraData(DataReader* reader) override;

    virtual std::string GetInputDescription(unsigned int index) const override;


private:

    std::string samplerName;
};