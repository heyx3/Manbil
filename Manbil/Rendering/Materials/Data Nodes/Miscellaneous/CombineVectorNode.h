#pragma once

#include "../DataNode.h"


//Combines multiple smaller floats/vectors into one large vector.
class CombineVectorNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "combineToMakeVector"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


    CombineVectorNode(const DataLine & input1, const DataLine & input2, std::string name = "") : CombineVectorNode(MakeVector(input1, input2), name) { }
    CombineVectorNode(const DataLine & input1, const DataLine & input2, const DataLine & input3,  std::string name = "") : CombineVectorNode(MakeVector(input1, input2, input3), name) { }
    CombineVectorNode(const DataLine & input1, const DataLine & input2, const DataLine & input3, const DataLine & input4, std::string name = "") : CombineVectorNode(MakeVector(input1, input2, input3, input4), name) { }
    CombineVectorNode(const std::vector<DataLine> & inputs, std::string name = "");


protected:

    virtual void WriteMyOutputs(std::string & outCode) const override;
};