#pragma once

#include "../DataNode.h"

//TODO: Pull out into .cpp.


//Different calculated values based on shader vertex inputs.


//The screen-space position of the given object-space position.
//Outputs two values:
//0: the screen position as a vec3.
//1: the screen position before the homogenous component division, as a vec4.
class ObjectPosToScreenPosCalcNode : public DataNode
{
public:

    static unsigned int GetPosOutputIndex(void) { return 0; }
    static unsigned int GetHomogenousPosOutputIndex(void) { return 1; }


    virtual std::string GetName(void) const override { return "objToScreenPosNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));

        return GetName() + ToString(GetUniqueID()) +
                  (index == 0 ? "_pos" : "_homogenousPos");
    }

    ObjectPosToScreenPosCalcNode(DataLine & objectPos)
        : DataNode(MakeVector(objectPos), MakeVector(3, 4)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex <= 1, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WVP_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};
//The screen-space normal of the given object-space normal.
class ObjectNormalToScreenNormalCalcNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "screenNormalNode"; }

    ObjectNormalToScreenNormalCalcNode(DataLine & objectNormal)
        : DataNode(MakeVector(objectNormal), MakeVector(3)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WVP_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};



//The world-space position of the given object-space position.
class ObjectPosToWorldPosCalcNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "objPosToWorldNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index));

        return GetName() + ToString(GetUniqueID()) + "_worldPos";
    }

    ObjectPosToWorldPosCalcNode(DataLine & objectPos)
        : DataNode(MakeVector(objectPos), MakeVector(3)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex <= 1, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WORLD_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};
//The screen-space normal of the given object-space normal.
class ObjectNormalToWorldNormalCalcNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "objNormalToWorldNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index));
        return GetName() + ToString(GetUniqueID()) + "_worldNormal";
    }

    ObjectNormalToWorldNormalCalcNode(DataLine & objNormal) : DataNode(MakeVector(objNormal), MakeVector(3)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WORLD_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};

//The screen-space position of the given world-space position.
class WorldPosToScreenPosCalcNode : public DataNode
{
public:

    static unsigned int GetPosOutputIndex(void) { return 0; }
    static unsigned int GetHomogenousPosOutputIndex(void) { return 1; }


    virtual std::string GetName(void) const override { return "worldPosToScreenNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index <= 1, std::string() + "Invalid output index " + std::to_string(index));
        return GetName() + ToString(GetUniqueID()) + "_screenPos" + (index == GetPosOutputIndex() ? "" : "_homogenous");
    }

    WorldPosToScreenPosCalcNode(DataLine & worldPos) : DataNode(MakeVector(worldPos), MakeVector(3, 4)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex <= 1, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_VIEWPROJ_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};
//The screen-space normal of the given world-space normal.
class WorldNormalToScreenNormalCalcNode : public DataNode
{
public:

    virtual std::string GetName(void) const override { return "worldNormalToScreenNode"; }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index));
        return GetName() + ToString(GetUniqueID()) + "_screenNormal";
    }

    WorldNormalToScreenNormalCalcNode(DataLine & worldNormal) : DataNode(MakeVector(worldNormal), MakeVector(3)) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_VIEWPROJ_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;
};