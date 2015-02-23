#include "GPUParticleNodes.h"

#include "../Materials/Data Nodes/DataNodes.hpp"


ADD_NODE_REFLECTION_DATA_CPP(PosInSphereNode, 1.0f, 1.0f, 1.0f, Vector3f(), 1.0f)
ADD_NODE_REFLECTION_DATA_CPP(PosInBoxNode, Vector3f(), Vector3f(), Vector3f());
ADD_NODE_REFLECTION_DATA_CPP(ConstAccelNode, Vector3f(), Vector3f(), Vector3f());


DataNode::Ptr RandSeed1Node = DataNode::Ptr(new ShaderInNode(4, "randSeed1Node", 1, 1, 1)),
              RandSeed2Node = DataNode::Ptr(new ShaderInNode(2, "randSeed2Node", 2, 2, 2));
DataNode::Ptr SplitRSeed1Node = DataNode::Ptr(new VectorComponentsNode(RandSeed1Node,
                                                                       "splitRSeed1Node"));
DataNode::Ptr SplitRSeed2Node = DataNode::Ptr(new VectorComponentsNode(RandSeed2Node,
                                                                       "splitRSeed2Node"));


DataLine GetRandSeedFloat(unsigned int seedIndex)
{
    switch (seedIndex)
    {
        case 0: return DataLine(SplitRSeed1Node, 0);
        case 1: return DataLine(SplitRSeed1Node, 1);
        case 2: return DataLine(SplitRSeed1Node, 2);
        case 3: return DataLine(SplitRSeed1Node, 3);
        case 4: return DataLine(SplitRSeed2Node, 0);
        case 5: return DataLine(SplitRSeed2Node, 1);

        default:
            assert(false);
            return DataLine();
    }
}
DataNode::Ptr GetRandSeeds(unsigned int randSeedIndex1, unsigned int randSeedIndex2)
{
    return DataNode::Ptr(new CombineVectorNode(GetRandSeedFloat(randSeedIndex1),
                                               GetRandSeedFloat(randSeedIndex2)));
}
DataNode::Ptr GetRandSeeds(unsigned int randSeedIndex1, unsigned int randSeedIndex2,
                           unsigned int randSeedIndex3)
{
    return DataNode::Ptr(new CombineVectorNode(GetRandSeedFloat(randSeedIndex1),
                                               GetRandSeedFloat(randSeedIndex2),
                                               GetRandSeedFloat(randSeedIndex3)));
}
DataNode::Ptr GetRandSeeds(unsigned int randSeedIndex1, unsigned int randSeedIndex2,
                           unsigned int randSeedIndex3, unsigned int randSeedIndex4)
{
    return DataNode::Ptr(new CombineVectorNode(GetRandSeedFloat(randSeedIndex1),
                                               GetRandSeedFloat(randSeedIndex2),
                                               GetRandSeedFloat(randSeedIndex3),
                                               GetRandSeedFloat(randSeedIndex4)));
}


PosInSphereNode::PosInSphereNode(const DataLine& randX, const DataLine& randY, const DataLine& randZ,
                                 const DataLine& spherePos, const DataLine& sphereRadius,
                                 std::string nodeName)
    : DataNode(MakeInputs(randX, randY, randZ, spherePos, sphereRadius))
{

}

#pragma warning(disable: 4100)
unsigned int PosInSphereNode::GetOutputSize(unsigned int index) const
{
    return 3;
}
#pragma warning(default: 4100)

std::string PosInSphereNode::GetOutputName(unsigned int index) const
{
    switch (index)
    {
        case 0: return GetName() + "_pos";
        case 1: return GetName() + "_towardPos";
        default:
            assert(false);
            return "INVALID_OUTPUT";
    }
}

std::string PosInSphereNode::GetInputDescription(unsigned int index) const
{
    switch (index)
    {
        case 0: return "X Dir";
        case 1: return "Y Dir";
        case 2: return "Z Dir";
        case 3: return "Sphere Pos";
        case 4: return "Sphere Radius";
        default:
            assert(false);
            return "INVALID_INPUT";
    }
}
void PosInSphereNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 1 &&
             GetInputs()[1].GetSize() == 1 &&
             GetInputs()[2].GetSize() == 1,
           "'randX', 'randY', and 'randZ' must all be size 1");
    Assert(GetInputs()[3].GetSize() == 3, "'spherePos' must be size 3");
    Assert(GetInputs()[4].GetSize() == 1, "'sphereRadius' must be size 1");
}
void PosInSphereNode::WriteMyOutputs(std::string& outCode) const
{
    std::string tempName = GetName() + "_unpackedDir";
    outCode += "\tvec3 " + tempName + " = vec3(" + GetInputs()[0].GetValue() + ", " +
                                                   GetInputs()[1].GetValue() + ", " +
                                                   GetInputs()[2].GetValue() + ");\n";
    outCode += '\t' + tempName + " = (" + tempName + " * 2.0f) - 1.0f;\n";

    outCode += "\tvec3 " + GetOutputName(1) + " = normalize(" + tempName + ");\n";
    outCode += "\tvec3 " + GetOutputName(0) + " = " + GetInputs()[3].GetValue() + " + (" +
                    GetOutputName(1) + " * " + GetInputs()[4].GetValue() + ");\n";
}

std::vector<DataLine> PosInSphereNode::MakeInputs(const DataLine& d1, const DataLine& d2,
                                                  const DataLine& d3, const DataLine& d4,
                                                  const DataLine& d5)
{
    std::vector<DataLine> dls;
    dls.push_back(d1);
    dls.push_back(d2);
    dls.push_back(d3);
    dls.push_back(d4);
    dls.push_back(d5);
    return dls;
}


PosInBoxNode::PosInBoxNode(const DataLine& boxMinCorner, const DataLine& boxMaxCorner,
                           const DataLine& posLerp, std::string nodeName)
    : DataNode(MakeVector(boxMinCorner, boxMaxCorner, posLerp))
{

}

#pragma warning(disable: 4100)
unsigned int PosInBoxNode::GetOutputSize(unsigned int index) const
{
    return 3;
}
#pragma warning(default: 4100)

#pragma warning(disable: 4100)
std::string PosInBoxNode::GetOutputName(unsigned int index) const
{
    return GetName() + "_pos";
}
#pragma warning(default: 4100)

std::string PosInBoxNode::GetInputDescription(unsigned int index) const
{
    switch (index)
    {
        case 0: return "Box Min Corner";
        case 1: return "Box Max Corner";
        case 2: return "Pos Lerp";
        default:
            assert(false);
            return "INVALID_INPUT";
    }
}
void PosInBoxNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 3 &&
             GetInputs()[1].GetSize() == 3 &&
             GetInputs()[2].GetSize() == 3,
           "All inputs should be size 3");
}
void PosInBoxNode::WriteMyOutputs(std::string& outCode) const
{
    outCode += "\tvec3 " + GetOutputName(1) + " = normalize(vec3(" + GetInputs()[0].GetValue() + ", " +
                                                                     GetInputs()[1].GetValue() + ", " +
                                                                     GetInputs()[2].GetValue() + "));\n";
}




ConstAccelNode::ConstAccelNode(const DataLine& startPos, const DataLine& startVel,
                               const DataLine& accel, DataLine currentTime,
                               std::string nodeName)
    : DataNode(MakeVector(startPos, startVel, accel, currentTime), nodeName)
{

}

#pragma warning(disable: 4100)
unsigned int ConstAccelNode::GetOutputSize(unsigned int index) const
{
    return 3;
}
#pragma warning(default: 4100)

std::string ConstAccelNode::GetOutputName(unsigned int index) const
{
    switch (index)
    {
        case 0:
            return GetName() + "_pos";
        case 1:
            return GetName() + "_vel";
        default:
            assert(false);
            return "INVALID_OUT_INDEX";
    }
}

void ConstAccelNode::AssertMyInputsValid(void) const
{
    Assert(GetInputs()[0].GetSize() == 3, "Initial position input must be size 3");
    Assert(GetInputs()[1].GetSize() == 3, "Initial velocity input must be size 3");
    Assert(GetInputs()[2].GetSize() == 3, "Acceleration input must be size 3");
    Assert(GetInputs()[3].GetSize() == 1, "Current time input must be size 1");
}
std::string ConstAccelNode::GetInputDescription(unsigned int index) const
{
    switch (index)
    {
        case 0: return "Starting Position";
        case 1: return "Starting Velocity";
        case 2: return "Acceleration";
        case 3: return "Current Time";

        default:
            assert(false);
            return "INVALID_IN_INDEX";
    }
}
void ConstAccelNode::WriteMyOutputs(std::string& outCode) const
{
    outCode += "\tvec3 " + GetOutputName(1) + " = " + GetInputs()[1].GetValue() +
                    " + (" + GetInputs()[3].GetValue() + " * " + GetInputs()[2].GetValue() + ");\n";
    outCode += "\tvec3 " + GetOutputName(0) + " = " + GetInputs()[0].GetValue() +
                    " + (" + GetInputs()[3].GetValue() + " * " + GetOutputName(1) + ");\n";
}