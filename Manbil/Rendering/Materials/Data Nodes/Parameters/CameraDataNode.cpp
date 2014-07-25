#include "CameraDataNode.h"

#include "../../MaterialData.h"


DataNodePtr CameraDataNode::instance = DataNodePtr(new CameraDataNode());


std::string CameraDataNode::GetOutputName(unsigned int index) const
{
    switch (index)
    {
        case 0: return MaterialConstants::CameraPosName;
        case 1: return MaterialConstants::CameraForwardName;
        case 2: return MaterialConstants::CameraUpName;
        case 3: return MaterialConstants::CameraSideName;

        default:
            Assert(false, std::string() + "Invalid output index " + ToString(index));
            return "UNKNOWN_CAMERA_DAT_NAME";
    }
}
void CameraDataNode::SetMyFlags(MaterialUsageFlags & flags, unsigned int outputIndex) const
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

        default: Assert(false, std::string() + "Invalid output index " + ToString(outputIndex));
    }
}