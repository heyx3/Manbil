#pragma once

#include <assert.h>


//Different kinds of materials.
enum RenderingModes
{
    RM_Opaque,
    RM_Transluscent,
    RM_Additive,
};
bool IsModeTransparent(RenderingModes mode) { return (mode == RM_Transluscent || mode == RM_Additive); }


//Data about how a material handles light.
//It will not use ambient occlusion if "IsLit" is false.
struct LightSettings
{
public:
    bool UseAmbientOcclusion;
    LightSettings(bool useAO) : UseAmbientOcclusion(useAO) { }
};


//Different kinds of data fundamental to the look of a material.
//Different channels will not be used depending on rendering mode and light settings.
//Each channel may be computed through a constant uniform, a texture lookup, or an expression/function call.
enum RenderingChannels
{
    RC_ObjectVertexOffset,
    RC_WorldVertexOffset,

    RC_Diffuse,
    RC_DiffuseIntensity,

    RC_Specular,
    RC_SpecularIntensity,

    RC_Normal,

    RC_Opacity,
    RC_Distortion,

    RC_NumbChannels,
};

//Gets whether the given channel is used in the given rendering mode with the given material light settings.
bool IsChannelUsed(RenderingChannels channel, RenderingModes mode, LightSettings settings, bool isLit)
{
    typedef RenderingChannels RCs;

    if (channel == RCs::RC_NumbChannels) return false;

    if (!isLit)
    {
        if (channel == RCs::RC_Specular || channel == RCs::RC_SpecularIntensity || channel == RCs::RC_Normal)
            return false;
    }

    switch (mode)
    {
    case RenderingModes::RM_Opaque:
        return (channel == RenderingChannels::RC_Opacity || channel == RenderingChannels::RC_Distortion);

    case RenderingModes::RM_Transluscent:
        return true;

    case RenderingModes::RM_Additive:
        return true;

    default: assert(false);
    }
}