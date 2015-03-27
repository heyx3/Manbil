#pragma once

#include "../DataNode.h"


//A singleton node.
//Outputs a vec2 representing the pixel coordinates of the current fragment.
//Divide by the width/height outputs of CameraDataNode to remap the coordinates to [0, 1].
//Only usable in the fragment shader.
class FragCoordNode : public DataNode
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


    FragCoordNode(void) : DataNode(std::vector<DataLine>(), "fragCoordNodeSingleton") { }


    ADD_NODE_REFLECTION_DATA_H(FragCoordNode)
};