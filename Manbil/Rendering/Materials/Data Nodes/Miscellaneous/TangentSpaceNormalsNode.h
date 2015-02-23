#pragma once

#include "../DataNode.h"


//A DataNode that takes in tangent-space normals and outputs the combination of them all.
class TangentSpaceNormalsNode : public DataNode
{
public:

    TangentSpaceNormalsNode(const std::vector<DataLine>& normals, std::string name = "")
        : DataNode(normals, name) { }
    TangentSpaceNormalsNode(DataLine normal1, DataLine normal2, std::string name = "")
        : DataNode(MakeVector(normal1, normal2), name) { }
    TangentSpaceNormalsNode(DataLine normal1, DataLine normal2, DataLine normal3, std::string name = "")
        : DataNode(MakeVector(normal1, normal2, normal3), name) { }


    virtual std::string GetOutputName(unsigned int outputIndex) const override;
    virtual unsigned int GetOutputSize(unsigned int outputIndex) const override;


protected:

    virtual void AssertMyInputsValid(void) const override;

    virtual void WriteMyOutputs(std::string& outCode) const;

    virtual std::string GetInputDescription(unsigned int index) const override;


    ADD_NODE_REFLECTION_DATA_H(TangentSpaceNormals);
};