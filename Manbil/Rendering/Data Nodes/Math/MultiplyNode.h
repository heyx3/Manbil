#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../Math/Lower Math/Vectors.h"


//Represents a simple multiplication operator.
class MultiplyNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    MultiplyNode(const std::vector<DataLine> & toMultiply, std::string name = "");
    MultiplyNode(DataLine toMultiply1, DataLine toMultiply2, std::string name = "") : MultiplyNode(MakeVector(toMultiply1, toMultiply2), name) { }
    MultiplyNode(DataLine toMultiply1, DataLine toMultiply2, DataLine toMultiply3, std::string name = "") : MultiplyNode(MakeVector(toMultiply1, toMultiply2, toMultiply3), name) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual void AssertMyInputsValid(void) const override;

    ADD_NODE_REFLECTION_DATA_H(MultiplyNode)
};