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


    virtual std::string GetTypeName(void) const override { return "objPosToScreen"; }

    virtual unsigned int GetNumbOutputs(void) const override { return 2; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));
        return (index == 0 ? 3 : 4);
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));
        return GetName() + (index == 0 ? "_screenPos" : "_homogenousScreenPos");
    }

    ObjectPosToScreenPosCalcNode(DataLine & objectPos, std::string _name)
        : DataNode(MakeVector(objectPos),
        [](std::vector<DataLine> & ins, std::string name) { return DataNodePtr(new ObjectPosToScreenPosCalcNode(ins[0], name)); },
        _name)
    {
        Assert(objectPos.GetSize() == 3, "Input 'objectPos' must have size 3, but it has size " + ToString(objectPos.GetSize()));
    }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex <= 1, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WVP_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return "objectPos";
    }
};
//The screen-space normal of the given object-space normal.
class ObjectNormalToScreenNormalCalcNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "objNormalToScreen"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return 3;
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return GetName() + "_screenNormal";
    }

    ObjectNormalToScreenNormalCalcNode(const DataLine & objectNormal, std::string name = "")
        : DataNode(MakeVector(objectNormal),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new ObjectNormalToScreenNormalCalcNode(ins[0], _name)); },
                   name)
    {
        Assert(objectNormal.GetSize() == 3, "Input 'objectNormal' must have size 3, but it has size " + ToString(objectNormal.GetSize()));
    }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WVP_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return "objectNormal";
    }
};



//The world-space position of the given object-space position.
class ObjectPosToWorldPosCalcNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "objPosToWorld"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return 3;
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return GetName() + "_worldPos";
    }

    ObjectPosToWorldPosCalcNode(DataLine & objectPos, std::string name = "")
        : DataNode(MakeVector(objectPos),
                   [](std::vector<DataLine> & in, std::string _name) { return DataNodePtr(new ObjectPosToWorldPosCalcNode(in[0], _name)); },
                   name) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WORLD_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return "objectPos";
    }
};
//The screen-space normal of the given object-space normal.
class ObjectNormalToWorldNormalCalcNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "objNormalToWorld"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return 3;
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + std::to_string(index));
        return GetName() + "_worldNormal";
    }

    ObjectNormalToWorldNormalCalcNode(DataLine & objNormal, std::string name = "")
        : DataNode(MakeVector(objNormal),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new ObjectNormalToWorldNormalCalcNode(ins[0], _name)); },
                   name) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_WORLD_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return "objectNormal";
    }
};

//The screen-space position of the given world-space position.
class WorldPosToScreenPosCalcNode : public DataNode
{
public:

    static unsigned int GetPosOutputIndex(void) { return 0; }
    static unsigned int GetHomogenousPosOutputIndex(void) { return 1; }


    virtual std::string GetTypeName(void) const override { return "worldPosToScreen"; }

    virtual unsigned int GetNumbOutputs(void) const override { return 2; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));
        return (index == 0 ? 3 : 4);
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index <= 1, std::string() + "Invalid output index " + ToString(index));
        return GetName() + (index == 0 ? "_screenPos" : "_homogenousScreenPos");
    }

    WorldPosToScreenPosCalcNode(DataLine & worldPos, std::string name = "")
        : DataNode(MakeVector(worldPos),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new WorldPosToScreenPosCalcNode(ins[0], _name)); },
                   name) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex <= 1, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_VIEWPROJ_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return "worldPos";
    }
};
//The screen-space normal of the given world-space normal.
class WorldNormalToScreenNormalCalcNode : public DataNode
{
public:

    virtual std::string GetTypeName(void) const override { return "worldNormalToScreen"; }

    virtual unsigned int GetOutputSize(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return 3;
    }
    virtual std::string GetOutputName(unsigned int index) const override
    {
        Assert(index == 0, std::string() + "Invalid output index " + ToString(index));
        return GetName() + "_screenNormal";
    }

    WorldNormalToScreenNormalCalcNode(DataLine & worldNormal, std::string name = "")
        : DataNode(MakeVector(worldNormal),
                   [](std::vector<DataLine> & ins, std::string _name) { return DataNodePtr(new WorldNormalToScreenNormalCalcNode(ins[0], _name)); },
                   name) { }


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        Assert(outputIndex == 0, std::string() + "Invalid output index " + ToString(outputIndex));
        flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_VIEWPROJ_MAT);
    }
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override
    {
        Assert(index == 0, "Invalid output index " + ToString(index));
        return "worldNormal";
    }
};