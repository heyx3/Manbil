#pragma once

#include <string>
#include "../../RenderingState.h"
#include "RenderingModes.h"
#include "../../ShaderHandler.h"
#include "UniformCollections.h"



//Constants/information used for the Material system.
struct MaterialConstants
{
public:
    static RenderingState GetRenderingState(RenderingModes mode);

    static const std::string ElapsedTimeName,
                             WorldMatName, ViewMatName, ProjMatName, WVPMatName,
                             CameraPosName, CameraForwardName, CameraUpName, CameraSideName,
                             InObjPos, InWorldPos, InUV, InObjNormal, InWorldNormal, InColor,
                             OutObjPos, OutWorldPos, OutUV, OutObjNormal, OutWorldNormal, OutColor,
                             FinalOutColor;
    static const std::string DirectionalLightName,
                             DirectionalLight_ColorName, DirectionalLight_DirName,
                             DirectionalLight_DiffuseName, DirectionalLight_AmbientName;

    static std::string GetVertexHeader(bool useLighting);
    static std::string GetFragmentHeader(bool useLighting);
    static std::string GetHeader(ShaderHandler::Shaders shaderType, bool useLighting)
    {
        switch (shaderType)
        {
            case ShaderHandler::Shaders::SH_Vertex_Shader: return GetVertexHeader(useLighting);
            case ShaderHandler::Shaders::SH_Fragment_Shader: return GetFragmentHeader(useLighting);

            default: assert(false); return "";
        }
    }
};

//Required data declarations at the top of all shaders.
struct ShaderHeaders
{
public:

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