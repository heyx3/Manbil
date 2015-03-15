#pragma once

#include "../DataNode.h"


//Takes in normal/tangent/bitangent and a tangent-space normal vector.
//Converts the normal vector into the same space as the given normal/tangent/bitangent.
class ApplyNormalMapNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


    ApplyNormalMapNode(const DataLine& normal, const DataLine& tangent, const DataLine& bitangent,
                       const DataLine& tangentSpaceNormal, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string& outCode) const override;

    virtual void AssertMyInputsValid(void) const override;

    ADD_NODE_REFLECTION_DATA_H(ApplyNormalMapNode)
};