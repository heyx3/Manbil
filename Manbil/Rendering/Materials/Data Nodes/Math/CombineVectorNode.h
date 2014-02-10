#pragma once

#include "../DataNode.h"


//Combines multiple smaller floats/vectors into one large vector.
class CombineVectorNode : public DataNode
{
public:

    virtual std::string GetName(void) const { return "combineVectorNode"; }

    CombineVectorNode(const std::vector<DataLine> & inputs) : DataNode(inputs, MakeVector(0))
    {
        count = 0;
        for (int i = 0; i < inputs.size(); ++i)
            count += inputs[i].GetDataLineSize();

        assert(count > 0 && count < 5);
    }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;

private:

    unsigned int count;
};