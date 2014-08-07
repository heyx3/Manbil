#pragma once

#include <assert.h>
#include "../DataNode.h"


//Represents a simple addition operator.
class AddNode : public DataNode
{
public:

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    AddNode(const std::vector<DataLine> & toAdd, std::string name = "");
    AddNode(DataLine toAdd1, DataLine toAdd2, std::string name = "") : AddNode(MakeVector(toAdd1, toAdd2), name) { }
    AddNode(DataLine toAdd1, DataLine toAdd2, DataLine toAdd3, std::string name = "") : AddNode(MakeVector(toAdd1, toAdd2, toAdd3), name) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual void AssertMyInputsValid(void) const override;

    MAKE_NODE_READABLE_H(AddNode)
};