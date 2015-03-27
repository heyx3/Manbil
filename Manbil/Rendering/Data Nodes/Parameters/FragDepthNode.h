#pragma once

#include "../DataNode.h"


//A singleton node.
//Outputs a float in the range {0, 1} representing the depth of the fragment being rendered.
//Only usable in the fragment shader.
class FragDepthNode : public DataNode
{
public:

    static DataNode::Ptr GetInstance(void) { return instance; }
    

    virtual std::string GetOutputName(unsigned int outputIndex) const override;
    virtual unsigned int GetOutputSize(unsigned int outputIndex) const override { return 1; }


protected:

    //Don't need to write any extra code.
    virtual void WriteMyOutputs(std::string& outCode) const override { }

    virtual void AssertMyInputsValid(void) const override;


private:

    static DataNode::Ptr instance;


    FragDepthNode(void) : DataNode(std::vector<DataLine>(), "fragDepthNodeSingleton") { }


    ADD_NODE_REFLECTION_DATA_H(FragDepthNode)
};