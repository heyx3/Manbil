#include "HGPOutputComponent.h"

#include "HGPComponentManager.h"


//Each component has a unique ID.
//This value cannot be static because HGPOutputComponent is a template, not a class.
unsigned int HGPGlobalData::NextHGPComponentID = 1;

//The exception that is thrown if an instance of this component fails a sanity check.
const int HGPGlobalData::EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT = 19285;


const DataNode::Ptr HGPGlobalData::_ParticleIDInputPtr = DataNode::Ptr(new ShaderInNode(2, "particleIDInputs", 0, 0, 0, 0)),
                  HGPGlobalData::_ParticleRandSeedInputs1Ptr = DataNode::Ptr(new ShaderInNode(4, "particleRandSeedInputs1", 1, 1, 0, 1)),
                  HGPGlobalData::_ParticleRandSeedInputs2Ptr = DataNode::Ptr(new ShaderInNode(2, "particleRandSeedInputs2", 2, 2, 0, 2));
const DataLine HGPGlobalData::ParticleIDInput = DataLine("particleIDInputs"),
               HGPGlobalData::ParticleRandSeedInputs1 = DataLine("particleRandSeedInputs1"),
               HGPGlobalData::ParticleRandSeedInputs2 = DataLine("particleRandSeedInputs2"),
               HGPGlobalData::ParticleUVs = DataLine(FragmentInputNode::GetInstance()->GetName(), 3);

const DataNode::Ptr HGPGlobalData::ParticleRandSeedComponents1 =
      DataNode::Ptr(new VectorComponentsNode(HGPGlobalData::ParticleRandSeedInputs1, "particleRandSeedComponents"));
const DataNode::Ptr HGPGlobalData::ParticleRandSeedComponents2 =
      DataNode::Ptr(new VectorComponentsNode(HGPGlobalData::ParticleRandSeedInputs2, "particleRandSeedComponents"));


DataLine HGPGlobalData::GetRandSeed(unsigned int randSeedIndex)
{
    if (randSeedIndex < 4)
        return DataLine(HGPGlobalData::ParticleRandSeedComponents1->GetName(), randSeedIndex);
    else if (randSeedIndex < 6)
        return DataLine(HGPGlobalData::ParticleRandSeedComponents2->GetName(), randSeedIndex - 4);
    else { assert(false); return DataLine(); }
}

const std::string HGPGlobalData::ParticleElapsedTimeUniformName = "u_particleTime";
const DataNode::Ptr _ParticleElapsedTimePtr = DataNode::Ptr(new ParamNode(1, HGPGlobalData::ParticleElapsedTimeUniformName, "particleElapsedTimeParam"));
const DataLine HGPGlobalData::ParticleElapsedTime = DataLine("particleElapsedTimeParam");


UniformDictionary & HGPGlobalData::GetParams(HGPComponentManager & manager) { return manager.Params; }
const DataLine & HGPGlobalData::GetTimeLerp(HGPComponentManager & manager) { return manager.GetTimeInterpolant(); }