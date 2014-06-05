#include "GPUParticleDefines.h"

#include "../Materials/Data Nodes/DataLine.h"


bool IsValidGPUPOutput(const DataLine & outputData, GPUPOutputs outputType)
{
    switch (outputType)
    {
        case GPUPOutputs::GPUP_WORLDPOSITION:
            return outputData.GetDataLineSize() == 3;
        case GPUPOutputs::GPUP_QUADROTATION:
            return outputData.GetDataLineSize() == 1;
        case GPUPOutputs::GPUP_COLOR:
            return outputData.GetDataLineSize() == 4;
        case GPUPOutputs::GPUP_SIZE:
            return outputData.GetDataLineSize() == 2;

        default: assert(false); return false;
    }
}