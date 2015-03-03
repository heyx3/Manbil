#include "GPUParticleDefines.h"

#include "../Data Nodes/DataLine.h"


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

RenderIOAttributes ParticleVertex::GetVertexInputData(void)
{
    return RenderIOAttributes(RenderIOAttributes::Attribute(2, false, "vIn_ParticleID"),
                              RenderIOAttributes::Attribute(4, false, "vIn_RandSeeds1"),
                              RenderIOAttributes::Attribute(2, false, "vIn_RandSeeds2"));
}
RenderIOAttributes ParticleVertex::GetGeoInputData(void)
{
    return RenderIOAttributes(RenderIOAttributes::Attribute(2, false, "vOut_ParticleID"),
                              RenderIOAttributes::Attribute(4, false, "vOut_RandSeeds1"),
                              RenderIOAttributes::Attribute(2, false, "vOut_RandSeeds2"));
}
RenderIOAttributes ParticleVertex::GetFragInputData(void)
{
    return RenderIOAttributes(RenderIOAttributes::Attribute(2, false, "gOut_ParticleID"),
                              RenderIOAttributes::Attribute(4, false, "gOut_RandSeeds1"),
                              RenderIOAttributes::Attribute(2, false, "gOut_RandSeeds2"),
                              RenderIOAttributes::Attribute(2, false, "gOut_UV"));
}