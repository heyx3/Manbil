#include "SpecialHGPComponents.h"


DataLine ConstantAccelerationHGPComponent::GenerateComponentOutput(void) const
{
    const DataLine & t = HGPGlobalData::ParticleElapsedTime;

    std::vector<DataLine> accelTerm;
    accelTerm.insert(accelTerm.end(), DataLine(0.5f));
    accelTerm.insert(accelTerm.end(), t);
    accelTerm.insert(accelTerm.end(), t);
    accelTerm.insert(accelTerm.end(), acceleration->GetComponentOutput());

    velocityMult = DataNodePtr(new MultiplyNode(t, initialVelocity->GetComponentOutput(), GetName() + "_velocityTerm"));
    accelMult = DataNodePtr(new MultiplyNode(accelTerm, GetName() + "_accelTerm"));

    addResult = DataNodePtr(new AddNode(initialPosition->GetComponentOutput(), DataLine(velocityMult->GetName()),
                                        DataLine(accelMult->GetName()), GetName() + "_addResult"));

    return DataLine(addResult->GetName());
}