#include "ProjectionDataNode.h"

#include "../../MaterialData.h"


std::string ProjectionDataNode::GetOutputName(unsigned int index) const
{
    switch (index)
    {
        case 0: return MaterialConstants::CameraWidthName;
        case 1: return MaterialConstants::CameraHeightName;
        case 2: return MaterialConstants::CameraZNearName;
        case 3: return MaterialConstants::CameraZFarName;
        case 4: return MaterialConstants::CameraFovName;

        default:
            Assert(false, std::string() + "Invalid output index " + ToString(index));
            return "INVALID_PROJECTION_DATA_NAME";
    }
}