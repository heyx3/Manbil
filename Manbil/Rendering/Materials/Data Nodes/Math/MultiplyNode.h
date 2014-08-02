#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple multiplication operator.
class MultiplyNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "multiplication"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    MultiplyNode(const std::vector<DataLine> & toMultiply, std::string name = "");
    MultiplyNode(DataLine toMultiply1, DataLine toMultiply2, std::string name = "") : MultiplyNode(MakeVector(toMultiply1, toMultiply2), name) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;
};