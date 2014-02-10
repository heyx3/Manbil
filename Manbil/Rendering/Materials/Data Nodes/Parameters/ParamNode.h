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

    virtual void GetMyParameterDeclarations(std::vector<std::string> & outDecls, Shaders shaderType) const override
    {
        outDecls.insert(outDecls.end(), Vector(vSize).GetGLSLType() + " " + GetUniformName());
    }

    virtual std::string GetOutputName(unsigned int outputIndex, Shaders shaderType) const override
    {
        assert(outputIndex == 0);
        return GetUniformName();
    }

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override
    {
        //No need to write any outputs; the uniform variable is the output.
    }


private:

    std::string GetUniformName(void) const { return GetName() + std::to_string(GetUniqueID()) + "_" + name; }

    std::string name;
    unsigned int vSize;
};