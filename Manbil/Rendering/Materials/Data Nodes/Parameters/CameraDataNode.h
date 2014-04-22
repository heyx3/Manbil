#pragma once

#include "../DataNode.h"


//Outputs different camera position, as well as forward/upwards/sideways vectors.
class CameraDataNode : public DataNode
{
public:

    static unsigned int GetCamPosOutputIndex(void) { return 0; }
    static unsigned int GetCamForwardOutputIndex(void) { return 1; }
    static unsigned int GetCamUpwardOutputIndex(void) { return 2; }
    static unsigned int GetCamSidewaysOutputIndex(void) { return 3; }

    virtual std::string GetName(void) const override { return "cameraDataNode"; }

    CameraDataNode(void) : DataNode(std::vector<DataLine>(), MakeVector(3, 3, 3, 3)) { }

    virtual std::string GetOutputName(unsigned int index) const override;

protected:

    virtual void SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const override
    {
        switch (outputIndex)
        {
        case 0:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_CAM_POS);
            break;
        case 1:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_CAM_FORWARD);
            break;
        case 2:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_CAM_UPWARDS);
            break;
        case 3:
            flags.EnableFlag(MaterialUsageFlags::Flags::DNF_USES_CAM_SIDEWAYS);
            break;

        default: Assert(false, std::string() + "Invalid output index " + std::to_string(outputIndex));
        }
    }

    virtual void WriteMyOutputs(std::string & outCode) const override
    {
        //No output writing needed.
    }
};