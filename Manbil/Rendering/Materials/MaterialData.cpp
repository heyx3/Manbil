#include "MaterialData.h"

#include <assert.h>


const std::string MaterialConstants::ElapsedTimeName = "u_elapsed_seconds",
                  MaterialConstants::WorldMatName = "u_matWorld",
                  MaterialConstants::ViewMatName = "u_matView",
                  MaterialConstants::ProjMatName = "u_matProj",
                  MaterialConstants::WVPMatName = "u_matWVP",
                  MaterialConstants::CameraPosName = "u_cam_pos",
                  MaterialConstants::CameraForwardName = "u_cam_forward",
                  MaterialConstants::CameraUpName = "u_cam_upward",
                  MaterialConstants::CameraSideName = "u_cam_sideways",
                  MaterialConstants::CameraWidthName = "u_cam_width",
                  MaterialConstants::CameraHeightName = "u_cam_height",
                  MaterialConstants::CameraZNearName = "u_cam_zNear",
                  MaterialConstants::CameraZFarName = "u_cam_zFar",
                  MaterialConstants::CameraFovName = "u_cam_fov",

                  //TODO: Once lighting is removed from shader generator, remove these light names.
                  MaterialConstants::DirectionalLightName = "u_dir_light",
                  MaterialConstants::DirectionalLight_ColorName = "u_dir_light.Col",
                  MaterialConstants::DirectionalLight_DirName = "u_dir_light.Dir",
                  MaterialConstants::DirectionalLight_AmbientName = "u_dir_light.Ambient",
                  MaterialConstants::DirectionalLight_DiffuseName = "u_dir_light.Diffuse",

                  MaterialConstants::InWorldPos = "in_world_pos",
                  MaterialConstants::InObjPos = "in_obj_pos",
                  MaterialConstants::InUV = "in_uv",
                  MaterialConstants::InWorldNormal = "in_world_normal",
                  MaterialConstants::InObjNormal = "in_obj_normal",
                  MaterialConstants::InColor = "in_color",
                  
                  MaterialConstants::VertexOutNameBase = "out_vertexOutput",
                  //MaterialConstants::OutNormalName = "out_vertexNormal",

                  MaterialConstants::FinalOutColor = "FinalOut_Color";

RenderingState MaterialConstants::GetRenderingState(RenderingModes mode)
{
    switch (mode)
    {
        case RenderingModes::RM_Opaque:
            return RenderingState();
        case RenderingModes::RM_Transluscent:
            //TODO: I think transluscent shouldn't write to the depth buffer, because then it removes the need for depth sorting? Test this.
            return RenderingState(RenderingState::Cullables::C_NONE);
        case RenderingModes::RM_Additive:
            return RenderingState(RenderingState::C_NONE, RenderingState::BE_ONE, RenderingState::BE_ONE, true, false);

        default:
            assert(false);
            return RenderingState();
    }
}
std::string MaterialConstants::GetUniformDeclarations(const MaterialUsageFlags & flags)
{
    typedef MaterialUsageFlags::Flags FL;

    std::string uniformDecls = (flags.GetFlag(FL::DNF_USES_TIME) ? "uniform float " + ElapsedTimeName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_CAM_POS) ? "uniform vec3 " + CameraPosName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_CAM_FORWARD) ? "uniform vec3 " + CameraForwardName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_CAM_UPWARDS) ? "uniform vec3 " + CameraUpName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_CAM_SIDEWAYS) ? "uniform vec3 " + CameraSideName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_WIDTH) ? "uniform float " + CameraWidthName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_HEIGHT) ? "uniform float " + CameraHeightName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_ZNEAR) ? "uniform float " + CameraZNearName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_ZFAR) ? "uniform float " + CameraZFarName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_FOV) ? "uniform float " + CameraFovName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_WORLD_MAT) ? "uniform mat4 " + WorldMatName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_VIEW_MAT) ? "uniform mat4 " + ViewMatName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_PROJ_MAT) ? "uniform mat4 " + ProjMatName + ";\n" : "") +
                               (flags.GetFlag(FL::DNF_USES_WVP_MAT) ? "uniform mat4 " + WVPMatName + ";\n" : "");
    if (!uniformDecls.empty())
        uniformDecls = std::string() + "//Uniforms.\n" + uniformDecls + "\n\n\n";

    if (false)
    {
        uniformDecls +=
"struct DirectionalLight                       n\
{                                             \n\
    vec3 Col, Dir;                            \n\
    float Ambient, Diffuse;                   \n\
};                                            \n\
uniform DirectionalLight " + DirectionalLightName + ";\n\
                                              \n\
vec3 getLight(vec3 surfaceNormal, vec3 fragToCamNormal, float specular, float specularIntensity, DirectionalLight lightDir)\n\
{\n\
    float dotted = max(dot(-surfaceNormal, lightDir.Dir), 0.0);\n\
    \n\
    vec3 lightReflect = normalize(reflect(lightDir.Dir, surfaceNormal));\n\
    \n\
    float specFactor = max(0.0, dot(fragToCamNormal, lightReflect));\n\
    specFactor = pow(specFactor, specularIntensity);\n\
    \n\
    return lightDir.Col * (lightDir.Ambient + (lightDir.Diffuse * dotted) + (specular * specFactor));\n\
}\n\n";
    }

    return uniformDecls;
}

std::string MaterialConstants::GetVertexHeader(std::string outputDeclarations, const MaterialUsageFlags & flags)
{
    return std::string() +
"#version 330                                                    \n\
                                                                 \n\
layout (location = 0) in vec3 " + InObjPos + ";                  \n\
layout (location = 1) in vec4 " + InColor + ";                   \n\
layout (location = 2) in vec2 " + InUV + ";                      \n\
layout (location = 3) in vec3 " + InObjNormal + ";               \n\
                                                                 \n\
" + outputDeclarations + "                                       \n\
                                                                 \n\
" + GetUniformDeclarations(flags);
}
std::string MaterialConstants::GetFragmentHeader(std::string inputDeclarations, std::string outputDeclarations, const MaterialUsageFlags & flags)
{
    return std::string() +
"#version 330                                                    \n\
                                                                 \n\
" + inputDeclarations + "                                        \n\
                                                                 \n\
" + outputDeclarations + "                                       \n\
                                                                 \n\
" + GetUniformDeclarations(flags);
}