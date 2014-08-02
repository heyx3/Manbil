#pragma once

#include <vector>
#include "../DataNode.h"
#include "../../MaterialData.h"
#include "../../../../Vertices.h"



#pragma warning(disable: 4100)

//Represents the value of a fragment input (from the vertex shader, or the geometry shader if it exists).
//This class is a singleton.
//NOTE: This node is only usable in the fragment shader!
class FragmentInputNode : public DataNode
{
public:

    static DataNodePtr GetInstance(void) { return instance; }


    virtual std::string GetTypeName(void) const override { return "fragmentInputs"; }

    virtual unsigned int GetNumbOutputs(void) const override;

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;


private:

    static DataNodePtr instance;

    FragmentInputNode(void);
};

#pragma warning(default: 4100)