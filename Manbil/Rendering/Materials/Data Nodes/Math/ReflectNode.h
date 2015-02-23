#pragma once

#include "../DataNode.h"


//Outputs a vector reflected around another vector.
class ReflectNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    ReflectNode(const DataLine & toReflect, const DataLine & reflectNormal, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void AssertMyInputsValid(void) const override;


    ADD_NODE_REFLECTION_DATA_H(ReflectNode)
};