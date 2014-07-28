#pragma once

#include <assert.h>
#include "../DataNode.h"


//Represents a simple addition operator.
class AddNode : public DataNode
{
public:

    AddNode(const std::vector<DataLine> & toAdd, std::string name = "");
    AddNode(DataLine toAdd1, DataLine toAdd2, std::string name = "");
    AddNode(DataLine toAdd1, DataLine toAdd2, DataLine toAdd3, std::string name = "");


protected:

    virtual void ResetOutputs(std::vector<unsigned int> & newOuts) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;
};