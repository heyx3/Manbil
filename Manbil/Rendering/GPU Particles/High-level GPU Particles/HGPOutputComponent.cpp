#include "HGPOutputComponent.h"

#include "HGPComponentManager.h"


//Each component has a unique ID, just like with DataNodes.
//This value cannot be static because HGPOutputComponent is a template, not a class.
unsigned int HGPGlobalData::NextHGPComponentID = 1;

//The exception that is thrown if an instance of this component fails a sanity check.
const int HGPGlobalData::EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT = 19285;


const DataLine HGPGlobalData::ParticleIDInput = DataLine(DataNodePtr(new ShaderInNode(2, 0, 0, 0, 0)), 0),
               HGPGlobalData::ParticleRandSeedInputs1 = DataLine(DataNodePtr(new ShaderInNode(4, 1, 1, 0, 1)), 0),
               HGPGlobalData::ParticleRandSeedInputs2 = DataLine(DataNodePtr(new ShaderInNode(2, 2, 2, 0, 2)), 0),
               HGPGlobalData::ParticleUVs = DataLine(DataNodePtr(new FragmentInputNode(VertexAttributes(2, 4, 2, 2, false, false, false, false))), 3);
const DataNodePtr HGPGlobalData::ParticleRandSeedComponents1 = DataNodePtr(new VectorComponentsNode(HGPGlobalData::ParticleRandSeedInputs1));
const DataNodePtr HGPGlobalData::ParticleRandSeedComponents2 = DataNodePtr(new VectorComponentsNode(HGPGlobalData::ParticleRandSeedInputs2));

DataLine HGPGlobalData::GetRandSeed(unsigned int randSeedIndex)
{
    if (randSeedIndex < 4)
        return DataLine(HGPGlobalData::ParticleRandSeedComponents1, randSeedIndex);
    else if (randSeedIndex < 6)
        return DataLine(HGPGlobalData::ParticleRandSeedComponents2, randSeedIndex - 4);
    else { assert(false); return DataLine(); }
}

const std::string HGPGlobalData::ParticleElapsedTimeUniformName = "u_particleTime";
const DataLine HGPGlobalData::ParticleElapsedTime = DataLine(DataNodePtr(new ParamNode(1, HGPGlobalData::ParticleElapsedTimeUniformName)), 0);


TextureManager & HGPGlobalData::GetTexManager(HGPComponentManager & manager) { return manager.Manager; }
UniformDictionary & HGPGlobalData::GetParams(HGPComponentManager & manager) { return manager.Params; }
const DataLine & HGPGlobalData::GetTimeLerp(HGPComponentManager & manager) { return manager.GetTimeInterpolant(); }