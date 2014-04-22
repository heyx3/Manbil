#pragma once

#include "../DataNode.h"


//Represents a DataNode that outputs some user-defined
//     vector that can be changed at run-time.
class ParamNode : public DataNode
{
public:

    ParamNode(unsigned int vectorSize, std::string glslName)
        : vSize(vectorSize), name(glslName), DataNode(std::vector<DataLine>(), MakeVector(vectorSize))
    {

    }

    virtual std::string GetName(void) const override { return "paramNode"; }

    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms) const override
    {
        float data[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        outUniforms.FloatUniforms[GetOutputName(0)] = UniformValueF(data, vSize, GetOutputName(0));
    }

    virtual std::string GetOutputName(unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + std::to_string(outputIndex));
        return name;
    }

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No need to write any outputs; the uniform variable is the output.
    }


private:

    std::string name;
    unsigned int vSize;
};