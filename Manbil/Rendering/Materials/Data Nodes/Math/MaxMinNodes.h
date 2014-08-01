#pragma once

#include "../DataNode.h"


//Outputs the max of an input.
class MaxNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "max"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return BasicMath::Max(GetInputs()[0].GetSize(), GetInputs()[1].GetSize());
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetName() + "_maxed";
    }

    MaxNode(const DataLine & in1, const DataLine & in2, std::string name = "")
        : DataNode(MakeVector(in1, in2),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new MaxNode(ins[0], ins[1])); },
                   name)
    {
        Assert(in1.GetSize() == 1 || in2.GetSize() == 1 ||
               in1.GetSize() == in2.GetSize(),
               "Both inputs need to be either size 1 or the same size!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
        outCode += "\t" + vecType + " " + GetOutputName(0) + " = max(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
    }
};

//Outputs the min of an input.
class MinNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "min"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return BasicMath::Max(GetInputs()[0].GetSize(), GetInputs()[1].GetSize());
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return GetName() + "_maxed";
    }

    MinNode(const DataLine & in1, const DataLine & in2, std::string name = "")
        : DataNode(MakeVector(in1, in2),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new MinNode(ins[0], ins[1])); },
                   name)
    {
        Assert(in1.GetSize() == 1 || in2.GetSize() == 1 ||
               in1.GetSize() == in2.GetSize(),
               "Both inputs need to be either size 1 or the same size!");
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        std::string vecType = VectorF(GetOutputSize(0)).GetGLSLType();
        outCode += "\t" + vecType + " " + GetOutputName(0) + " = min(" + GetInputs()[0].GetValue() + ", " + GetInputs()[1].GetValue() + ");\n";
    }
};