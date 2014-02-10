#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple addition operator.
class AddNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "additionNode"; }

    AddNode(const std::vector<DataLine> & toAdd);
    AddNode(DataLine toAdd1, DataLine toAdd2);


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;
};