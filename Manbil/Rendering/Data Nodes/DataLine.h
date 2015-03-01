#pragma once

#include <memory>
#include <assert.h>
#include "../Basic Rendering/GLVectors.h"
#include "../../IO/DataSerialization.h"


class DataNode;

//Represents an input into a DataNode. Its value is either a constant or the output of a DataNode.
struct DataLine : public ISerializable
{
public:

    //Creates a DataLine that gets its input value from a DataNode.
    DataLine(std::string nodeName, unsigned int outputLineIndex = 0)
        : isConstantValue(false), nonConstantValue(nodeName),
          nonConstantOutputIndex(outputLineIndex) { }
    //Creates a DataLine that gets its input value from a DataNode.
    DataLine(std::shared_ptr<DataNode> nodePtr, unsigned int outputLineIndex = 0);

    //Creates a DataLine with a constant input value.
    DataLine(const VectorF& constantInput) : isConstantValue(true), constantValue(constantInput) { }
    //Creates a DataLine with a constant float value.
    DataLine(float value) : isConstantValue(true), constantValue(value) { }
    //Creates a DataLine with a constant vec2 value.
    DataLine(Vector2f value) : isConstantValue(true), constantValue(value) { }
    //Creates a DataLine with a constant vec3 value.
    DataLine(Vector3f value) : isConstantValue(true), constantValue(value) { }
    //Creates a DataLine with a constant vec4 value.
    DataLine(Vector4f value) : isConstantValue(true), constantValue(value) { }

    //Creates a DataLine with the default constant value { 0, 0, 0, 0 }.
    DataLine(void) : DataLine(Vector4f(0.0f, 0.0f, 0.0f, 0.0f)) { }


    //Does this DataLine have a constant value?
    bool IsConstant(void) const { return isConstantValue; }

    //Does this DataLine have the given constant value?
    bool IsConstant(float val) const { return IsConstant() && constantValue == val; }
    //Does this DataLine have the given constant value?
    bool IsConstant(Vector2f val) const { return IsConstant() && constantValue == val; }
    //Does this DataLine have the given constant value?
    bool IsConstant(Vector3f val) const { return IsConstant() && constantValue == val; }
    //Does this DataLine have the given constant value?
    bool IsConstant(Vector4f val) const { return IsConstant() && constantValue == val; }


    //Gets the number of floats in this data line.
    unsigned int GetSize(void) const;

    //This function is only valid if this DataLine has a constant input value.
    VectorF GetConstantValue(void) const { assert(isConstantValue); return constantValue; }

    //This function is only valid if this DataLine has a DataNode input value.
    std::string GetNonConstantValue(void) const;
    //This function is only valid if this DataLine has a DataNode input value.
    unsigned int GetNonConstantOutputIndex(void) const;

    //Assumes this DataLine isn't constant.
    DataNode* GetNode(void) const;

    //Gets the GLSL expression for this data line output.
    std::string GetValue(void) const;


    virtual void WriteData(DataWriter* writer) const override;
    virtual void ReadData(DataReader* reader) override;


private:

    bool isConstantValue;

    VectorF constantValue;

    std::string nonConstantValue;
    unsigned int nonConstantOutputIndex;
};