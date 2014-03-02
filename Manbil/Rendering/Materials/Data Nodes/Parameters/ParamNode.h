#pragma once

#include "../DataNode.h"


//Represents a DataNode that outputs some user-defined
//     parameter that can be changed at run-time.
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
        outUniforms.FloatUniforms[GetOutputName(0)] = UniformValueF(0.0f, GetOutputName(0));
    }

    virtual std::string GetOutputName(unsigned int outputIndex) const override
    {
        assert(outputIndex == 0);
        return GetName() + std::to_string(GetUniqueID()) + "_" + name;
    }

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No need to write any outputs; the uniform variable is the output.
    }


private:

    std::string name;
    unsigned int vSize;
};