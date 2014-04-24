#pragma once

#include <string>
#include "../../RenderingState.h"
#include "RenderingModes.h"
#include "../../ShaderHandler.h"
#include "UniformCollections.h"
#include "MaterialUsageFlags.h"



//Constants/information used for the Material system.
struct MaterialConstants
{
public:
    static RenderingState GetRenderingState(RenderingModes mode);

    static const std::string ElapsedTimeName,
                             WorldMatName, ViewMatName, ProjMatName, WVPMatName,
                             CameraPosName, CameraForwardName, CameraUpName, CameraSideName,
                             CameraZNearName, CameraZFarName, CameraWidthName, CameraHeightName, CameraFovName,
                             VertexOutNameBase,//OutNormalName,
                             InObjPos, InWorldPos, InUV, InObjNormal, InWorldNormal, InColor,
                             FinalOutColor;
    static const std::string DirectionalLightName,
                             DirectionalLight_ColorName, DirectionalLight_DirName,
                             DirectionalLight_DiffuseName, DirectionalLight_AmbientName;
    
    //Gets the uniform declarations for a material with the given usage flags.
    static std::string GetUniformDeclarations(const MaterialUsageFlags & flags);

    static std::string GetVertexHeader(std::string outputDeclarations, const MaterialUsageFlags & flags);
    static std::string GetFragmentHeader(std::string inputDeclarations, std::string outputDeclarations, const MaterialUsageFlags & flags);
};


//The different rendering passes.
//TODO: Change this (regular render pass, emissive render pass, surface normal render pass, etc). Allow for creation of shaders that output to multiple framebuffers at once.
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