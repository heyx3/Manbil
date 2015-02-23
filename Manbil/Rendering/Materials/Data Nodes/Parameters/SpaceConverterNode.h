#pragma once

#include "../DataNode.h"


//Converts between different rendering coordinate spaces.
//Can convert normal vectors and positions.
//If converting a normal vector, there is only one output -- the converted vector.
//If converting a position, there are two outputs --
//    the actual pos (vec3) and the homogenous-coordinates pos (vec4), in that order.
//NOTE: data can only be converted "forwards" towards screen space, not "backwards" towards object space.
class SpaceConverterNode : public DataNode
{
public:

    //A commonly-used version of this node.
    static std::shared_ptr<DataNode> ObjPosToScreenPos(DataLine inP, std::string name = "")
    {
        return std::shared_ptr<DataNode>(new SpaceConverterNode(inP, ST_OBJECT, ST_SCREEN, DT_POSITION, name));
    }
    //A commonly-used version of this node.
    static std::shared_ptr<DataNode> ObjPosToWorldPos(DataLine inP, std::string name = "")
    {
        return std::shared_ptr<DataNode>(new SpaceConverterNode(inP, ST_OBJECT, ST_WORLD, DT_POSITION, name));
    }


    //The different coordinate spaces.
    enum SpaceTypes
    {
        //Relative to the object being rendered.
        ST_OBJECT,
        //Relative to the world origin.
        ST_WORLD,
        //Relative to the camera.
        ST_VIEW,
        //Relative to the screen.
        ST_SCREEN,
    };

    //The different types of data that can be converted between different coordinate spaces.
    enum DataTypes
    {
        //Normal vector.
        DT_NORMAL,
        //Position.
        DT_POSITION,
    };


    SpaceTypes SrcSpace, DestSpace;
    DataTypes DataType;


    virtual unsigned int GetNumbOutputs(void) const override;

    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;

    SpaceConverterNode(const DataLine & inVector, SpaceTypes srcSpace, SpaceTypes destSpace,
                       DataTypes inDataType, std::string name = "");


protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override;
    virtual void WriteMyOutputs(std::string & outCode) const override;

    virtual std::string GetInputDescription(unsigned int index) const override;
    
    virtual void WriteExtraData(DataWriter* writer) const override;
    virtual void ReadExtraData(DataReader* reader) override;

    virtual void AssertMyInputsValid(void) const override;


private:

    //Asserts that the src space is "closer" to object-space than the dest space.
    //If the assert fails, sets the error message and returns false.
    bool AssertValidSrcDestSpaces(std::string& outError) const;

    std::string ST_ToString(SpaceTypes st) const;
    std::string DT_ToString(DataTypes dt) const;

    static bool ST_FromString(std::string str, SpaceTypes & outSt);
    static bool DT_FromString(std::string str, DataTypes & outDt);


    ADD_NODE_REFLECTION_DATA_H(SpaceConverterNode)
};