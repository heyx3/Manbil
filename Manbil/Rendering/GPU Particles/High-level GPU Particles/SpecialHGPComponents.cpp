#include "SpecialHGPComponents.h"

DataLine KinematicsHGPComponent::GenerateComponentOutput(void) const
{
    DataLine t(DataNodePtr(new InterpolateNode(DataLine(0.0f), Duration->GetComponentOutput(), HGPGlobalData::ParticleTimeLerp, DataLine(1.0f))), 0);

    std::vector<DataLine> accelTerm;
    accelTerm.insert(accelTerm.end(), DataLine(0.5f));
    accelTerm.insert(accelTerm.end(), t);
    accelTerm.insert(accelTerm.end(), t);
    accelTerm.insert(accelTerm.end(), Acceleration->GetComponentOutput());

    return DataLine(DataNodePtr(new AddNode(InitialPosition->GetComponentOutput(),
                                            DataLine(DataNodePtr(new MultiplyNode(t, InitialVelocity->GetComponentOutput())), 0),
                                            DataLine(DataNodePtr(new MultiplyNode(accelTerm)), 0))), 0);
}