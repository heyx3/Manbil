#pragma once


#include "../../IO/DataSerialization.h"


//The type of simple data that can be used in place of a DAGNode.
//Using "void" for this value results in a DATInput with no default value type.
template<typename DefaultValueType>
//Represents an input to a DAG node.
//May be either a DAGNode output or some kind of default value type.
class DAGInput : public ISerializable
{
public:


    DAGInput(std::string dagNodeName = "", unsigned int dagNodeOutIndex = 0)
        : nonConstantName(dagNodeName), nonConstantOutIndex(dagNodeOutIndex) { }
    DAGInput(const DefaultValueType & constantVal)
        : constantValue(constantVal), nonConstantName(""), nonConstantOutIndex(0) { }

    virtual ~DAGInput(void) { }


    //Gets whether this input is a constant value (as opposed to a DAG node output).
    bool IsConstant(void) const { return nonConstantName.empty(); }
    //Gets whether this input is constant, and equal to the given value.
    bool IsConstant(const DefaultValueType & toCompare)) const { return IsConstant() && constantValue == toCompare; }


    //Gets the value of this input, assuming it's constant.
    const DefaultValueType & GetConstantValue(void) const { assert(IsConstant()); return constantValue; }
    //Gets the DAG node this input points to, assuming this input isn't constant.
    const std::string & GetDAGNodeName(void) const { assert(!IsConstant()); return nonConstantName; }
    //Gets the DAG node's output index, assuming this input isn't constant.
    unsigned int GetDAGNodeOutputIndex(void) const { assert(!IsConstant()); return nonConstantOutIndex; }


    //Sets the value of this input to the given constant value.
    //If it was already pointing to a DAG node output, that will be overwritten.
    void SetConstantInput(const DefaultValueType & defVal) { nonConstantName = ""; constantValue = defVal; }
    //Sets the value of this input to the given DAG node.
    void SetDAGNodeInput(const std::string & nodeName, unsigned int nodeOutputIndex)
    {
        nonConstantName = nodeName;
        nonConstantOutIndex = nodeOutputIndex;
    }


private:

    std::string nonConstantName;
    unsigned int nonConstantOutIndex;

    DefaultValueType constantValue;
};



//Template specialization for inputs with no default value type.

template<>
class DAGInput<void>
{
    
};