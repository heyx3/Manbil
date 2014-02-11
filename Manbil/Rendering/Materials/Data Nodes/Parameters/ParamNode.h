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

    virtual void GetMyParameterDeclarations(UniformDictionary & outUniforms, Shaders shaderType) const override
    {
        switch (GetOutputs()[0])
        {
            case 1:
                outUniforms.FloatUniforms[GetUniformName()] = UniformValue(0.0f, 0, GetUniformName());
            break;

            case 2:
                outUniforms.FloatUniforms[GetUniformName()] = UniformValue(Vector2f(0.0f, 0.0f), 0, GetUniformName());
            break;

            case 3:
                outUniforms.FloatUniforms[GetUniformName()] = UniformValue(Vector3f(0.0f, 0.0f, 0.0f), 0, GetUniformName());
            break;

            case 4:
                outUniforms.FloatUniforms[GetUniformName()] = UniformValue(Vector4f(0.0f, 0.0f, 0.0f, 0.0f), 0, GetUniformName());
            break;

        default: assert(false);
        }
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