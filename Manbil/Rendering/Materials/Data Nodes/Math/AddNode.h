#pragma once

#include <assert.h>
#include "../DataNode.h"
#include "../../../../Math/Vectors.h"


//Represents a simple addition operator.
/*
    Input lines:
    0: Vector, any size
    Output lines:
    0: Vector, same size as input
*/
class AddNode : public DataNode
{
public:

    virtual std::string GetName(void) override { return "addition DataNode"; }

    AddNode(DataLine toAdd)
        : DataNode(MakeVector(toAdd), MakeVector(toAdd.GetDataLineSize()))
    {

    }


    virtual std::string WriteMyOutputConnection(unsigned int dataLine, std::string inputName, std::string outputName) const override;


private:

    bool addsConstant;
    float addConstant[4];
};