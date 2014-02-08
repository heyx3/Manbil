#pragma once


enum RenderingModes
{
    RM_Opaque,
    RM_Transluscent,
    RM_Additive,
};

enum OpaqueRenderingChannels
{
    ORC_Diffuse,
    ORC_DiffuseIntensity,
    ORC_Specular,
    ORC_SpecularIntensity,

    ORC_Normal,

    ORC_NumbChannels,
};
enum TranslucentRenderingChannels
{
    TRC_Diffuse,
    TRC_DiffuseIntensity,
    TRC_Specular,
    TRC_SpecularIntensity,

    TRC_Opacity,
    TRC_Distortion,

    TRC_Normal,

    TRC_NumbChannels,
};
enum AdditiveRenderingChannels
{
    ARC_Diffuse,
    ARC_DiffuseIntensity,
    ARC_Specular,
    ARC_SpecularIntensity,

    TRC_Opacity,
    TRC_Distortion,

    ARC_Normal,

    ARC_NumbChannels,
};
