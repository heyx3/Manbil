#pragma once


//The GPU particle system in Manbil piggy-backs off of the DataNode system for generating materials.
class DataLine;

//Different output channels for GPU particle data.
enum GPUPOutputs
{
    //Size: 3.
    GPUP_WORLDPOSITION,
    //Size: 1.
    GPUP_QUADROTATION,
    //Size: 4.
    GPUP_COLOR,
    //Size: 2.
    GPUP_SIZE,
};

//Gets whether the given output line is a valid size for the given GPU particle data type.
bool IsValidGPUPOutput(const DataLine & outputData, GPUPOutputs outputType);