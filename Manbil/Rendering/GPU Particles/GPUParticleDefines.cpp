#include "GPUParticleDefines.h"

#include "../Materials/Data Nodes/DataLine.h"


bool IsValidGPUPOutput(const DataLine & outputData, GPUPOutputs outputType)
{
    switch (outputType)
    {
        case GPUPOutputs::GPUP_WORLDPOSITION:
            return outputData.GetSize() == 3;
        case GPUPOutputs::GPUP_QUADROTATION:
            return outputData.GetSize() == 1;
        case GPUPOutputs::GPUP_COLOR:
            return outputData.GetSize() == 4;
        case GPUPOutputs::GPUP_SIZE:
            return outputData.GetSize() == 2;

        default: assert(false); return false;
    }
}