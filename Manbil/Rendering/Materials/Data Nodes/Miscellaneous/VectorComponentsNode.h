#pragma once

#include "../DataNode.h"


//Splits a vector into its components.
//The outputs are the 0-based indices of the input vector (0 = x, 1 = y, etc.).
class VectorComponentsNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "vectorComponents"; }

    virtual unsigned int GetNumbOutputs(void) const override { return GetInputs()[0].GetSize(); }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


    VectorComponentsNode(const DataLine & inData, std::string name = "")
        : DataNode(MakeVector(inData),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new VectorComponentsNode(ins[0], _name)); },
                   name) { }


protected:

#pragma warning(disable: 4100)
    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No need to write anything; we're just pulling out components of another vector.
    }
#pragma warning(default: 4100)

    virtual std::string GetInputDescription(unsigned int index) const override;
};