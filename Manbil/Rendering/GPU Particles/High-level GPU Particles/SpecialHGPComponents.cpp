#include "SpecialHGPComponents.h"

DataLine ConstantAccelerationHGPComponent::GenerateComponentOutput(void) const
{
    const DataLine & t = HGPGlobalData::ParticleElapsedTime;

    std::vector<DataLine> accelTerm;
    accelTerm.insert(accelTerm.end(), DataLine(0.5f));
    accelTerm.insert(accelTerm.end(), t);
    accelTerm.insert(accelTerm.end(), t);
    accelTerm.insert(accelTerm.end(), acceleration->GetComponentOutput());

    return DataLine(DataNodePtr(new AddNode(initialPosition->GetComponentOutput(),
                                            DataLine(DataNodePtr(new MultiplyNode(t, initialVelocity->GetComponentOutput())), 0),
                                            DataLine(DataNodePtr(new MultiplyNode(accelTerm)), 0))), 0);
}