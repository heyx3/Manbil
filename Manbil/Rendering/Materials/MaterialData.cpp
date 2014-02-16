#include "MaterialData.h"


const std::string MaterialConstants::ElapsedTimeName = "u_elapsed_seconds",
                  MaterialConstants::WorldMatName = "u_matWorld",
                  MaterialConstants::ViewMatName = "u_matView",
                  MaterialConstants::ProjMatName = "u_matProj",
                  MaterialConstants::WVPMatName = "u_matWVP",
                  MaterialConstants::CameraPosName = "u_cam_pos",
                  MaterialConstants::CameraForwardName = "u_cam_forward",
                  MaterialConstants::CameraUpName = "u_cam_upward",
                  MaterialConstants::CameraSideName = "u_cam_sideways",

                  MaterialConstants::InPos = "in_pos",
                  MaterialConstants::InUV = "in_uv",
                  MaterialConstants::InNormal = "in_normal",
                  MaterialConstants::InColor = "in_color",

                  MaterialConstants::OutPos = "out_pos",
                  MaterialConstants::OutUV = "out_uv",
                  MaterialConstants::OutNormal = "out_normal",
                  MaterialConstants::OutColor = "out_color",
                  
                  MaterialConstants::FinalOutColor = "finalout_color";

RenderingState MaterialConstants::GetRenderingState(RenderingModes mode)
{
    switch (mode)
    {
    case RenderingModes::RM_Opaque:
        return RenderingState(true, false, true);
    case RenderingModes::RM_Transluscent:
        return RenderingState(true, true, true);
    case RenderingModes::RM_Additive:
        return RenderingState(true, true, true, RenderingState::Cullables::C_NONE,
                              RenderingState::BlendingExpressions::One, RenderingState::BlendingExpressions::One);

    default: assert(false);
    }
}
