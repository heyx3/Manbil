#include "RenderingModes.h"

#include <assert.h>


bool IsModeTransparent(RenderingModes mode)
{
    return (mode == RM_Transluscent || mode == RM_Additive);
}

std::string ChannelToString(RenderingChannels channel)
{
    switch (channel)
    {
        case RenderingChannels::RC_Color: return "Color";
        case RenderingChannels::RC_Opacity: return "Opacity";
        case RenderingChannels::RC_VertexPosOutput: return "VertexPosOutput";

        case RenderingChannels::RC_COLOR_OUT_2:
        case RenderingChannels::RC_COLOR_OUT_3:
        case RenderingChannels::RC_COLOR_OUT_4:
            return "ColorOutput" + std::to_string(GetColorOutputNumber(channel));

        case RenderingChannels::RC_VERTEX_OUT_0:
        case RenderingChannels::RC_VERTEX_OUT_1:
        case RenderingChannels::RC_VERTEX_OUT_2:
        case RenderingChannels::RC_VERTEX_OUT_3:
        case RenderingChannels::RC_VERTEX_OUT_4:
        case RenderingChannels::RC_VERTEX_OUT_5:
        case RenderingChannels::RC_VERTEX_OUT_6:
        case RenderingChannels::RC_VERTEX_OUT_7:
        case RenderingChannels::RC_VERTEX_OUT_8:
        case RenderingChannels::RC_VERTEX_OUT_9:
        case RenderingChannels::RC_VERTEX_OUT_10:
        case RenderingChannels::RC_VERTEX_OUT_11:
        case RenderingChannels::RC_VERTEX_OUT_12:
        case RenderingChannels::RC_VERTEX_OUT_13:
        case RenderingChannels::RC_VERTEX_OUT_14:
        case RenderingChannels::RC_VERTEX_OUT_15:
        case RenderingChannels::RC_VERTEX_OUT_16:
            return "VertexOutput" + std::to_string(GetVertexOutputNumber(channel));

        default: assert(false); return "UNKNOWN_CHANNEL";
    }
}

bool IsChannelVertexOutput(RenderingChannels channel, bool includeInvalidOutput)
{
    typedef RenderingChannels RCs;

    switch (channel)
    {
        case RCs::RC_VertexPosOutput:
        case RCs::RC_Color:
        case RCs::RC_Opacity:
        case RCs::RC_COLOR_OUT_2:
        case RCs::RC_COLOR_OUT_3:
        case RCs::RC_COLOR_OUT_4:
            return false;

        case RCs::RC_VERTEX_OUT_INVALID:
            return includeInvalidOutput;

        case RCs::RC_VERTEX_OUT_0:
        case RCs::RC_VERTEX_OUT_1:
        case RCs::RC_VERTEX_OUT_2:
        case RCs::RC_VERTEX_OUT_3:
        case RCs::RC_VERTEX_OUT_4:
        case RCs::RC_VERTEX_OUT_5:
        case RCs::RC_VERTEX_OUT_6:
        case RCs::RC_VERTEX_OUT_7:
        case RCs::RC_VERTEX_OUT_8:
        case RCs::RC_VERTEX_OUT_9:
        case RCs::RC_VERTEX_OUT_10:
        case RCs::RC_VERTEX_OUT_11:
        case RCs::RC_VERTEX_OUT_12:
        case RCs::RC_VERTEX_OUT_13:
        case RCs::RC_VERTEX_OUT_14:
        case RCs::RC_VERTEX_OUT_15:
        case RCs::RC_VERTEX_OUT_16:
            return true;

        default:
            assert(false);
            return false;
    }
}
bool IsChannelColorOutput(RenderingChannels channel, bool includeNormalOutput)
{
    typedef RenderingChannels RCs;

    switch (channel)
    {
        case RCs::RC_VertexPosOutput:
        case RCs::RC_Opacity:
        case RCs::RC_VERTEX_OUT_INVALID:
        case RCs::RC_VERTEX_OUT_0:
        case RCs::RC_VERTEX_OUT_1:
        case RCs::RC_VERTEX_OUT_2:
        case RCs::RC_VERTEX_OUT_3:
        case RCs::RC_VERTEX_OUT_4:
        case RCs::RC_VERTEX_OUT_5:
        case RCs::RC_VERTEX_OUT_6:
        case RCs::RC_VERTEX_OUT_7:
        case RCs::RC_VERTEX_OUT_8:
        case RCs::RC_VERTEX_OUT_9:
        case RCs::RC_VERTEX_OUT_10:
        case RCs::RC_VERTEX_OUT_11:
        case RCs::RC_VERTEX_OUT_12:
        case RCs::RC_VERTEX_OUT_13:
        case RCs::RC_VERTEX_OUT_14:
        case RCs::RC_VERTEX_OUT_15:
        case RCs::RC_VERTEX_OUT_16:
            return false;

        case RCs::RC_Color:
            return includeNormalOutput;

        case RCs::RC_COLOR_OUT_2:
        case RCs::RC_COLOR_OUT_3:
        case RCs::RC_COLOR_OUT_4:
            return true;

        default:
            assert(false);
            return false;
    }
}
bool IsChannelUsed(RenderingChannels channel, RenderingModes mode, LightSettings settings, bool isLit)
{
    typedef RenderingChannels RCs;

    switch (channel)
    {
        case RCs::RC_Color:
        case RCs::RC_VertexPosOutput:
        //Opacity is always used because you can still output to alpha channel even if the surface doesn't have transparency.
        case RCs::RC_Opacity:
            return true;


        default:
            assert(IsChannelVertexOutput(channel, false) || IsChannelColorOutput(channel, false));
            return true;
    }
}
int GetVertexOutputNumber(RenderingChannels vertOutput)
{
    typedef RenderingChannels RCs;

    switch (vertOutput)
    {
        case RCs::RC_VERTEX_OUT_0:
            return 0;
        case RCs::RC_VERTEX_OUT_1:
            return 1;
        case RCs::RC_VERTEX_OUT_2:
            return 2;
        case RCs::RC_VERTEX_OUT_3:
            return 3;
        case RCs::RC_VERTEX_OUT_4:
            return 4;
        case RCs::RC_VERTEX_OUT_5:
            return 5;
        case RCs::RC_VERTEX_OUT_6:
            return 6;
        case RCs::RC_VERTEX_OUT_7:
            return 7;
        case RCs::RC_VERTEX_OUT_8:
            return 8;
        case RCs::RC_VERTEX_OUT_9:
            return 9;
        case RCs::RC_VERTEX_OUT_10:
            return 10;
        case RCs::RC_VERTEX_OUT_11:
            return 11;
        case RCs::RC_VERTEX_OUT_12:
            return 12;
        case RCs::RC_VERTEX_OUT_13:
            return 13;
        case RCs::RC_VERTEX_OUT_14:
            return 14;
        case RCs::RC_VERTEX_OUT_15:
            return 15;
        case RCs::RC_VERTEX_OUT_16:
            return 16;

        default:
            assert(false);
            return -1;
    }
}
unsigned int GetColorOutputNumber(RenderingChannels colorOutput)
{
    typedef RenderingChannels RCs;

    switch (colorOutput)
    {
        case RCs::RC_Color:
            return 1;
        case RCs::RC_COLOR_OUT_2:
            return 2;
        case RCs::RC_COLOR_OUT_3:
            return 3;
        case RCs::RC_COLOR_OUT_4:
            return 4;

        default:
            assert(false);
            return 0;
    }
}