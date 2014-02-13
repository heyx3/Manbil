#pragma once

#include <string>
#include "../../RenderingState.h"
#include "RenderingModes.h"



//Constants/information used for the Material system.
struct MaterialConstants
{
public:
    static RenderingState GetRenderingState(RenderingModes mode);

    static const std::string ElapsedTimeName,
                             WorldMatName, ViewMatName, ProjMatName, WVPMatName,
                             CameraPosName, CameraForwardName, CameraUpName, CameraSideName,
                             InPos, InUV, InNormal, InColor,
                             OutPos, OutUV, OutNormal, OutColor;

};


//The different rendering passes.
enum RenderPasses
{
    //Render normals, depth, diffuse, etc. -- basic data, multiple render targets.
    BaseComponents = 0,
    //Render basic lighting, along with ambient occlusion -- two different render targets.
    //If ambient occlusion isn't enabled, the ambient occlusion render target won't be rendered to.
    CombineComponents,
    //Apply ambient occlusion. Obviously this pass is only done if ambient occlusion is done.
    ApplyOcclusion,

    //The total number of different render passes.
    Numb_Passes,
};