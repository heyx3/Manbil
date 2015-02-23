#pragma once


#include "../Materials/Data Nodes/Parameters/TimeNode.h"
#include "GPUParticleDefines.h"


//Defines several DataNodes that are useful for GPU particles.

//TODO: When parameterizing the number of rand seed values, turn the below global instances/functions into members of a class.

//The random seeds. These nodes' outputs are available in all shaders.
extern DataNode::Ptr RandSeed1Node, RandSeed2Node;
//Splits the random seed inputs up into individual nodes so that
extern DataNode::Ptr SplitRSeed1Node, SplitRSeed2Node;

//Gets a single random float from the particle vertices' "RandSeeds" attributes.
//Assumes that the given index is less than the total number of rand seeds to choose from.
DataLine GetRandSeedFloat(unsigned int randSeedIndex);
//Takes the given random floats from the particle vertices' "RandSeeds" attributes
//    and combines them into a new vector.
DataNode::Ptr GetRandSeeds(unsigned int randSeedIndex1, unsigned int randSeedIndex2);
//Takes the given random floats from the particle vertices' "RandSeeds" attributes
//    and combines them into a new vector.
DataNode::Ptr GetRandSeeds(unsigned int randSeedIndex1, unsigned int randSeedIndex2,
                           unsigned int randSeedIndex3);
//Takes the given random floats from the particle vertices' "RandSeeds" attributes
//    and combines them into a new vector.
DataNode::Ptr GetRandSeeds(unsigned int randSeedIndex1, unsigned int randSeedIndex2,
                           unsigned int randSeedIndex3, unsigned int randSeedIndex4);



//Takes in random X, Y, and Z values between 0 and 1
//    and gets a point on the surface of a sphere
//    that points from the sphere's center outward in that direction.
//Has two outputs:
// 0 -- the point in/on the sphere.
// 1 -- a normalized vector pointing from the center of the sphere to the generated point.
class PosInSphereNode : public DataNode
{
public:

    //The "normX" through "normZ" parameters do not have to represent a normalized vector.
    PosInSphereNode(const DataLine& dirX, const DataLine& dirY, const DataLine& dirZ,
                    const DataLine& spherePos, const DataLine& sphereRadius,
                    std::string nodeName = "");


    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void WriteMyOutputs(std::string& outCode) const override;
    virtual void AssertMyInputsValid(void) const override;


private:

    std::vector<DataLine> MakeInputs(const DataLine& d1, const DataLine& d2, const DataLine& d3,
                                     const DataLine& d4, const DataLine& d5);


    ADD_NODE_REFLECTION_DATA_H(PosInSphereNode);
};


//Generates a position inside a rectangular volume of space
//    given the volume bounds and lerp values for each axis.
class PosInBoxNode : public DataNode
{
public:

    PosInBoxNode(const DataLine& boxMinCorner, const DataLine& boxMaxCorner, const DataLine& posLerp,
                 std::string nodeName = "");


    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void WriteMyOutputs(std::string& outCode) const override;
    virtual void AssertMyInputsValid(void) const override;


    ADD_NODE_REFLECTION_DATA_H(PosInBoxNode);
};

//Takes in acceleration and initial position/velocity and uses kinematics to output the following data:
// 0 -- the current position.
// 1 -- the current velocity.
//Both outputs are size 3.
class ConstAccelNode : public DataNode
{
public:

    ConstAccelNode(const DataLine& startPos, const DataLine& startVel, const DataLine& accel,
                   DataLine currentTime = TimeNode::GetInstance(),
                   std::string nodeName = "");


    virtual unsigned int GetOutputSize(unsigned int index) const override;
    virtual std::string GetOutputName(unsigned int index) const override;


protected:

    virtual std::string GetInputDescription(unsigned int index) const override;

    virtual void WriteMyOutputs(std::string& outCode) const override;
    virtual void AssertMyInputsValid(void) const override;


    ADD_NODE_REFLECTION_DATA_H(ConstAccelNode);
};