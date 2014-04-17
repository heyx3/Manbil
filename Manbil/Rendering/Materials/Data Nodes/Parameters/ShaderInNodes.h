#pragma once

#include "../DataNode.h"
#include "../../MaterialData.h"


//A data node that just reads in from a shader input
//   (vertex/fragment color, uv, normal, or position).
class ShaderInNode : public DataNode
{
public:

    ShaderInNode(std::string vertexInput, std::string fragmentInput, unsigned int size)
        : vertIn(vertexInput), fragIn(fragmentInput), DataNode(std::vector<DataLine>(), MakeVector(size)) { }

    virtual std::string GetOutputName(unsigned int outputIndex) const override;

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No need to write any output; the "output" for this node is just a vertex input.
    }


protected:

    std::string GetVertexInput(void) const { return vertIn; }
    std::string GetFragmentInput(void) const { return fragIn; }

private:

    std::string vertIn, fragIn;
};

//A node that outputs the object-space position of the current vertex/fragment.
class ObjectPosNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "objectPosNode"; }
    ObjectPosNode(void) : ShaderInNode(MaterialConstants::InObjPos, MaterialConstants::OutObjPos, 3) { }
protected:
    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_OBJ_POS);
    }
};
//A node that outputs the world-space position of the current vertex/fragment.
class WorldPosNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "worldPosNode"; }
    WorldPosNode(void) : ShaderInNode(MaterialConstants::InWorldPos, MaterialConstants::OutWorldPos, 3) { }
protected:
    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WORLD_POS);
    }
};

//A node that outputs the uv coordinates at the current vertex/fragment.
class UVNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "uvNode"; }
    UVNode(void) : ShaderInNode(MaterialConstants::InUV, MaterialConstants::OutUV, 2) { }
protected:
    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_UV);
    }
};

//A node that outputs the color at the current vertex/fragment.
class ObjectColorNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "objectColorNode"; }
    ObjectColorNode(void) : ShaderInNode(MaterialConstants::InColor, MaterialConstants::OutColor, 4) { }
protected:
    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_COLOR);
    }
};

//A node that outputs the object-space normal at the current vertex/fragment.
class ObjectNormalNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "objectNormalNode"; }
    ObjectNormalNode(void) : ShaderInNode(MaterialConstants::InObjNormal, MaterialConstants::OutObjNormal, 3) { }
protected:
    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_OBJ_NORMAL);
    }
};
//A node that outputs the world-space normal at the current vertex/fragment.
class WorldNormalNode : public ShaderInNode
{
public:
    virtual std::string GetName(void) const override { return "worldNormalNode"; }
    WorldNormalNode(void) : ShaderInNode(MaterialConstants::InWorldNormal, MaterialConstants::OutWorldNormal, 3) { }
protected:
    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WORLD_NORMAL);
    }
};