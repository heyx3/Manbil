#include "RenderingModes.h"

bool IsModeTransparent(RenderingModes mode)
{
    return (mode == RM_Transluscent || mode == RM_Additive);
}

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