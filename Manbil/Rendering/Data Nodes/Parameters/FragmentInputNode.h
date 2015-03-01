#pragma once

#include <vector>
#include "../DataNode.h"
#include "../../Basic Rendering/MaterialConstants.h"
#include "../../Basic Rendering/Vertices.h"



#pragma warning(disable: 4100)

//Represents the value of a fragment input (from the vertex shader, or the geometry shader if it exists).
//This class is a singleton.
//NOTE: This node is only usable in the fragment shader!
class FragmentInputNode : public DataNode
{
public:

    static std::string GetInstanceName(void) { return "fragmentInputs"; }
    static std::shared_ptr<DataNode> GetInstance(void) { return instance; }


    virtual unsigned int GetNumbOutputs(void) const override;

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;


private:

    static std::shared_ptr<DataNode> instance;

    FragmentInputNode(void);
    
    ADD_NODE_REFLECTION_DATA_H(FragmentInputNode)
};

#pragma warning(default: 4100)