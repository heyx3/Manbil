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

                  MaterialConstants::DirectionalLightName = "u_dir_light",
                  MaterialConstants::DirectionalLight_ColorName = "u_dir_light.Col",
                  MaterialConstants::DirectionalLight_DirName = "u_dir_light.Dir",
                  MaterialConstants::DirectionalLight_AmbientName = "u_dir_light.Ambient",
                  MaterialConstants::DirectionalLight_DiffuseName = "u_dir_light.Diffuse",

                  MaterialConstants::InPos = "in_pos",
                  MaterialConstants::InUV = "in_uv",
                  MaterialConstants::InNormal = "in_normal",
                  MaterialConstants::InColor = "in_color",

                  MaterialConstants::OutPos = "out_pos",
                  MaterialConstants::OutUV = "out_uv",
                  MaterialConstants::OutNormal = "out_normal",
                  MaterialConstants::OutColor = "out_color",
                  
                  MaterialConstants::FinalOutColor = "FinalOut_Color";

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

std::string MaterialConstants::GetVertexHeader(bool useLighting)
{
    std::string ret = std::string() +
"#version 330                                                    \n\
                                                                 \n\
layout (location = 0) in vec3 " + InPos + ";                     \n\
layout (location = 1) in vec4 " + InColor + ";                   \n\
layout (location = 2) in vec2 " + InUV + ";                      \n\
layout (location = 3) in vec3 " + InNormal + ";                  \n\
                                                                 \n\
out vec3 " + OutPos + ";                                         \n\
out vec4 " + OutColor + ";                                       \n\
out vec2 " + OutUV + ";                                          \n\
out vec3 " + OutNormal + ";                                      \n\
                                                                 \n\
uniform float " + ElapsedTimeName + ";                           \n\
uniform vec3 " + CameraPosName + ", " +
                 CameraForwardName + ", " +
                 CameraUpName + ", " +
                 CameraSideName + ";                             \n\
uniform mat4 " + WorldMatName + ", " +
                 ViewMatName + ", " +
                 ProjMatName + ", " +
                 WVPMatName + ";\n\n";

    if (useLighting)
    {
        ret += "\n\
struct DirectionalLight                       \n\
{                                             \n\
    vec3 Col, Dir;                            \n\
    float Ambient, Diffuse;                   \n\
};                                            \n\
uniform DirectionalLight " + DirectionalLightName + ";\n\
                                              \n\
vec3 getLight(vec3 surfaceNormal, vec3 camToFragNormal, float specular, float specularIntensity, DirectionalLight lightDir)\n\
{\n\
    float dotted = max(dot(-surfaceNormal, lightDir.Dir), 0.0);\n\
    \n\
    vec3 fragToCam = -camToFragNormal;\n\
    vec3 lightReflect = normalize(reflect(lightDir.Dir, surfaceNormal));\n\
    \n\
    float specFactor = max(0.0, dot(fragToCam, lightReflect));\n\
    specFactor = pow(specFactor, specularIntensity);\n\
    \n\
    return lightDir.Col * (lightDir.Ambient + (lightDir.Diffuse * dotted) + (specular * specFactor));\n\
}\n";
    }

    return ret;
}
std::string MaterialConstants::GetFragmentHeader(bool useLighting)
{
    std::string ret = std::string() +
"#version 330                                                    \n\
                                                                 \n\
in vec3 " + OutPos + ";                                          \n\
in vec4 " + OutColor + ";                                        \n\
in vec2 " + OutUV + ";                                           \n\
in vec3 " + OutNormal + ";                                       \n\
                                                                 \n\
out vec4 " + FinalOutColor + ";                                  \n\
                                                                 \n\
uniform float " + ElapsedTimeName + ";                           \n\
uniform vec3 " + CameraPosName + ", " +
                 CameraForwardName + ", " +
                 CameraUpName + ", " +
                 CameraSideName + ";                             \n\
uniform mat4 " + WorldMatName + ", " +
                 ViewMatName + ", " +
                 ProjMatName + ", " +
                 WVPMatName + ";\n\n";

    
    if (useLighting)
    {
        ret += "\n\
struct DirectionalLight                       \n\
{                                             \n\
    vec3 Col, Dir;                            \n\
    float Ambient, Diffuse;                   \n\
};                                            \n\
uniform DirectionalLight " + DirectionalLightName + ";\n\
                                              \n\
vec3 getLight(vec3 surfaceNormal, vec3 camToFragNormal, float specular, float specularIntensity, DirectionalLight lightDir)\n\
{\n\
    float dotted = max(dot(-surfaceNormal, lightDir.Dir), 0.0);\n\
    \n\
    vec3 fragToCam = -camToFragNormal;\n\
    vec3 lightReflect = normalize(reflect(lightDir.Dir, surfaceNormal));\n\
    \n\
    float specFactor = max(0.0, dot(fragToCam, lightReflect));\n\
    specFactor = pow(specFactor, specularIntensity);\n\
    \n\
    return lightDir.Col * (lightDir.Ambient + (lightDir.Diffuse * dotted) + (specular * specFactor));\n\
}\n";
    }

    return ret;
}