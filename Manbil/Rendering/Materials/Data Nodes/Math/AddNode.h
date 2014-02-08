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
class AddDataNode : public DataNode
{
public:

    AddDataNode(DataLine toAdd)
        : DataNode()


private:

    unsigned int lines;

    bool addsConstant;
    float addConstant[4];
};