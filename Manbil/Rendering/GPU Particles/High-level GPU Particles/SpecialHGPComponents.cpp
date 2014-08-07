#include "SpecialHGPComponents.h"


DataLine ConstantAccelerationHGPComponent::GenerateComponentOutput(void) const
{
    const DataLine & t = HGPGlobalData::ParticleElapsedTime;

    std::vector<DataLine> accelTerm;
    accelTerm.insert(accelTerm.end(), DataLine(0.5f));
    accelTerm.insert(accelTerm.end(), t);
    accelTerm.insert(accelTerm.end(), t);
    accelTerm.insert(accelTerm.end(), acceleration->GetComponentOutput());

    velocityMult = DataNode::Ptr(0);
    accelMult = DataNode::Ptr(0);
    addResult = DataNode::Ptr(0);

    velocityMult = DataNode::Ptr(new MultiplyNode(t, initialVelocity->GetComponentOutput(), GetName() + "_velocityTerm"));
    accelMult = DataNode::Ptr(new MultiplyNode(accelTerm, GetName() + "_accelTerm"));

    addResult = DataNode::Ptr(new AddNode(initialPosition->GetComponentOutput(), DataLine(velocityMult->GetName()),
                                          DataLine(accelMult->GetName()), GetName() + "_addResult"));

    return DataLine(addResult->GetName());
}