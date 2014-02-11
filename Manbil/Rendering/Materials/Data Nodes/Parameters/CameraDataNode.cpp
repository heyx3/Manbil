#include "CameraDataNode.h"

#include "../../MaterialData.h"


std::string CameraDataNode::GetOutputName(unsigned int index, Shaders shaderType) const
{
    switch (index)
    {
        case 0: return MaterialConstants::CameraPosName;
        case 1: return MaterialConstants::CameraForwardName;
        case 2: return MaterialConstants::CameraUpName;
        case 3: return MaterialConstants::CameraSideName;

        default: assert(false);
    }
}