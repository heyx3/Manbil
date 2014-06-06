#pragma once

#include "HGPOutputComponent.h"


//Manages several HGPOutputComponents that together define a particle system's behavior.
class HGPComponentManager
{
public:

    HGPComponentPtr(3) WorldPosition;
    HGPComponentPtr(1) Rotation;
    HGPComponentPtr(2) Size;
    HGPComponentPtr(4) Color;

    HGPComponentManager(HGPComponentPtr(3) worldPos, HGPComponentPtr(1) rot, HGPComponentPtr(2) size, HGPComponentPtr(4) col)
    : WorldPosition(worldPos), Rotation(rot), Size(size), Color(col)
    {

    }

    void Initialize(void) { WorldPosition->InitializeComponent(); Rotation->InitializeComponent(); Size->InitializeComponent(); Color->InitializeComponent(); }
    void Update(void) { WorldPosition->UpdateComponent(); Rotation->UpdateComponent(); Size->UpdateComponent(); Color->UpdateComponent(); }

    //Puts DataLines into the given output map to create particles that behave according to this manager's components.
    void SetGPUPOutputs(std::unordered_map<GPUPOutputs, DataLine> & outputs)
    {
        outputs[GPUPOutputs::GPUP_WORLDPOSITION] = WorldPosition->GetComponentOutput();
        outputs[GPUPOutputs::GPUP_QUADROTATION] = Rotation->GetComponentOutput();
        outputs[GPUPOutputs::GPUP_SIZE] = Size->GetComponentOutput();
        outputs[GPUPOutputs::GPUP_COLOR] = Color->GetComponentOutput();
    }
};