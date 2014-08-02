#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple division operator.
class DivideNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "division"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    DivideNode(DataLine baseValue, const std::vector<DataLine> & toDivide, std::string name = "");
    DivideNode(DataLine baseValue, DataLine toDivide, std::string name = "") : DivideNode(baseValue, MakeVector(toDivide), name) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;
};