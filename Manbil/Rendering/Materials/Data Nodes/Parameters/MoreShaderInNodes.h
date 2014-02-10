#pragma once

#include "../DataNode.h"
#include "ShaderInNode.h"

//Some secondary shader input stuff.


//The world position of the vertex/fragment position.
class WorldPosNode : public DataNode
{
public:

    virtual std::string GetName(void) const { return "worldPosNode"; }

    WorldPosNode(void) : DataNode(MakeVector(DataLine(DataNodePtr(new ObjectPosNode()), 0)), MakeVector(0))
    {

    }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;
};
//The screen position of the vertex/fragment position.
class ScreenPosNode : public DataNode
{
public:

    virtual std::string GetName(void) const { return "screenPosNode"; }

    ScreenPosNode(void) : DataNode(MakeVector(DataLine(DataNodePtr(new ObjectPosNode()), 0)), MakeVector(0)) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;
};


//The world position of the vertex/fragment normal.
class WorldNormalNode : public DataNode
{
public:

    virtual std::string GetName(void) const { return "worldNormalNode"; }

    WorldNormalNode(void) : DataNode(MakeVector(DataLine(DataNodePtr(new ObjectNormalNode()), 0)), MakeVector(0))
    {

    }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;
};
//The screen position of the vertex/fragment normal.
class ScreenNormalNode : public DataNode
{
public:

    virtual std::string GetName(void) const { return "screenNormalNode"; }

    ScreenNormalNode(void) : DataNode(MakeVector(DataLine(DataNodePtr(new ObjectNormalNode()), 0)), MakeVector(0)) { }


protected:

    virtual void WriteMyOutputs(std::string & outCode, Shaders shaderType) const override;
};