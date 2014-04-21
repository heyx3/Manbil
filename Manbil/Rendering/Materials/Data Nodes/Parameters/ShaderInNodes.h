#pragma once

#include "../DataNode.h"
#include "../../MaterialData.h"


//TODO: Better names for this file (VertexInputNodes) and these node classes (e.x. ObjectPosVertexInNode).


//A data node that just reads in from a vertex input. Only applicable for vertex shaders!
class ShaderInNode : public DataNode
{
public:
    ShaderInNode(std::string inputName, unsigned int size)
        : input(inputName), DataNode(std::vector<DataLine>(), MakeVector(size)) { }
    virtual std::string GetOutputName(unsigned int outputIndex) const override
    {
        assert(outputIndex == 0);
        assert(GetShaderType() == Shaders::SH_Vertex_Shader);
        return input;
    }

protected:
    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        assert(GetShaderType() == Shaders::SH_Vertex_Shader);
        //No need to write any output; the "output" for this node is just a vertex input.
    }

private:
    std::string input;
};

//A node that outputs the object-space position of the current vertex.
class ObjectPosNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "objectPosNode"; }
    ObjectPosNode(void) : ShaderInNode(MaterialConstants::InObjPos, 3) { }
};

//A node that outputs the uv coordinates at the current vertex.
class UVNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "uvNode"; }
    UVNode(void) : ShaderInNode(MaterialConstants::InUV, 2) { }
};

//A node that outputs the color at the current vertex.
class ObjectColorNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "objectColorNode"; }
    ObjectColorNode(void) : ShaderInNode(MaterialConstants::InColor, 4) { }
};

//A node that outputs the object-space normal at the current vertex.
class ObjectNormalNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "objectNormalNode"; }
    ObjectNormalNode(void) : ShaderInNode(MaterialConstants::InObjNormal, 3) { }
};