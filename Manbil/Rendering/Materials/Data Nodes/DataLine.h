#pragma once

#include <memory>
#include "Vector.h"
#include "../../../ShaderHandler.h"
#include <assert.h>


class DataNode;
typedef std::shared_ptr<DataNode> DataNodePtr;


//Represents an input into a DataNode.
struct DataLine
{
public:

    //Creates a DataLine that gets its input value from a DataNode.
    DataLine(DataNodePtr input, unsigned int outputLineIndex)
        : isConstantValue(false), nonConstantValue(input), nonConstantValueIndex(outputLineIndex) { }
    //Creates a DataLine with a constant input value.
    DataLine(const Vector & constantInput) : isConstantValue(true), constantValue(constantInput) { }
    //Creates a DataLine with the default value { 0, 0, 0, 0 }.
    DataLine(void) : isConstantValue(true), constantValue(Vector(Vector4f(0.0f, 0.0f, 0.0f, 0.0f))) { }

    //This data line either feeds in a constant input, or gets the input value from a DataNode's output.
    bool IsConstant(void) const { return isConstantValue; }

    //Gets the number of floats in this data line.
    unsigned int GetDataLineSize(void) const;

    //This function is only valid if this DataLine has a constant input value.
    Vector GetConstantValue(void) const { assert(isConstantValue); return constantValue; }
    //This function is only valid if this DataLine has a DataNode input value.
    DataNodePtr GetDataNodeValue(void) const { assert(!isConstantValue); return nonConstantValue; }

    //Assuming this data line has a DataNode input value, gets the index of the DataNode input's data line.
    unsigned int GetDataNodeLineIndex(void) const { assert(!isConstantValue); return nonConstantValueIndex; }


    //Gets the GLSL expression for this data line output.
    std::string GetValue(void) const;


private:

    bool isConstantValue;

    Vector constantValue;

    DataNodePtr nonConstantValue;
    unsigned int nonConstantValueIndex;
};
