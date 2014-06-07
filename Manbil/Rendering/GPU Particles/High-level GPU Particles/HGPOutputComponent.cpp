#include "HGPOutputComponent.h"

#include "HGPComponentManager.h"


//Each component has a unique ID, just like with DataNodes.
//This value cannot be static because HGPOutputComponent is a template, not a class.
unsigned int HGPGlobalData::NextHGPComponentID = 1;

//The exception that is thrown if an instance of this component fails a sanity check.
const int HGPGlobalData::EXCEPTION_CHRONOLOGICAL_HGP_COMPONENT = 19285;


const DataLine HGPGlobalData::ParticleIDInput = DataLine(DataNodePtr(new ShaderInNode(2, 0, 0, 0, 0)), 0),
               HGPGlobalData::ParticleRandSeedInputs = DataLine(DataNodePtr(new ShaderInNode(3, 1, 1, 0, 1)), 0),
               HGPGlobalData::ParticleUVs = DataLine(DataNodePtr(new FragmentInputNode(VertexAttributes(2, 3, 2, false, false, false))), 2);
const DataNodePtr HGPGlobalData::ParticleRandSeedComponents = DataNodePtr(new VectorComponentsNode(HGPGlobalData::ParticleRandSeedInputs));
const DataLine HGPGlobalData::FourthRandSeed = DataLine(DataNodePtr(new WhiteNoiseNode(HGPGlobalData::ParticleRandSeedInputs, DataLine(43.2462f))), 0),
               HGPGlobalData::FifthRandSeed = DataLine(DataNodePtr(new WhiteNoiseNode(DataLine(DataNodePtr(
                                                new SwizzleNode(HGPGlobalData::ParticleRandSeedInputs,
                                                                SwizzleNode::Components::C_Y,
                                                                SwizzleNode::Components::C_Z,
                                                                SwizzleNode::Components::C_X)), 0),
                                                DataLine(43.2462f))), 0);

const std::string HGPGlobalData::ParticleElapsedTimeUniformName = "u_particleTime";
const DataLine HGPGlobalData::ParticleElapsedTime = DataLine(DataNodePtr(new ParamNode(1, HGPGlobalData::ParticleElapsedTimeUniformName)), 0);


TextureManager & HGPGlobalData::GetTexManager(HGPComponentManager & manager) { return manager.Manager; }
UniformDictionary & HGPGlobalData::GetParams(HGPComponentManager & manager) { return manager.Params; }
const DataLine & HGPGlobalData::GetTimeLerp(HGPComponentManager & manager) { return manager.GetTimeInterpolant(); }