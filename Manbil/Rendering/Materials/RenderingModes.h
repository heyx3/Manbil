#pragma once

#include <string>


//Different kinds of materials.
enum RenderingModes
{
    RM_Opaque,
    RM_Transluscent,
    RM_Additive,
};
bool IsModeTransparent(RenderingModes mode);


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
//TODO: Move into its own file inside DataNode folder.
enum RenderingChannels
{
    //Output position of vertex. Should always be in screen space
    //    (unless a geometry shader is being used).
    RC_VertexPosOutput,

    //Base color of surface.
    RC_Color,

    //Transparency of surface.
    RC_Opacity,

    //An "invalid" vertex output (interpolated fragment input).
    RC_VERTEX_OUT_INVALID,
    //Vertex output.
    RC_VERTEX_OUT_0,
    //Vertex output.
    RC_VERTEX_OUT_1,
    //Vertex output.
    RC_VERTEX_OUT_2,
    //Vertex output.
    RC_VERTEX_OUT_3,
    //Vertex output.
    RC_VERTEX_OUT_4,
    //Vertex output.
    RC_VERTEX_OUT_5,
    //Vertex output.
    RC_VERTEX_OUT_6,
    //Vertex output.
    RC_VERTEX_OUT_7,
    //Vertex output.
    RC_VERTEX_OUT_8,
    //Vertex output.
    RC_VERTEX_OUT_9,
    //Vertex output.
    RC_VERTEX_OUT_10,
    //Vertex output.
    RC_VERTEX_OUT_11,
    //Vertex output.
    RC_VERTEX_OUT_12,
    //Vertex output.
    RC_VERTEX_OUT_13,
    //Vertex output.
    RC_VERTEX_OUT_14,
    //Vertex output.
    RC_VERTEX_OUT_15,
    //Vertex output.
    RC_VERTEX_OUT_16,

    //Secondary color output.
    RC_COLOR_OUT_2,
    //Secondary color output.
    RC_COLOR_OUT_3,
    //Secondary color output.
    RC_COLOR_OUT_4,
};
std::string ChannelToString(RenderingChannels channel);

//Gets whether the given channel is a vertex output.
//"includeInvalidOutput" indicates what to return if the value is RC_VERTEX_OUT_INVALID.
bool IsChannelVertexOutput(RenderingChannels channel, bool includeInvalidOutput);
//Gets whether the given channel is a vertex output.
//"includeNormalOutput" indicates what to return if the value is RC_COLOR.
bool IsChannelColorOutput(RenderingChannels channel, bool includeNormalOutput);
//Gets whether the given channel is used in the given rendering mode with the given material light settings.
bool IsChannelUsed(RenderingChannels channel, RenderingModes mode, LightSettings settings, bool isLit);
//Gets the index of the given vertex output channel (returns -1 for RC_VERTEX_OUT_INVALID).
int GetVertexOutputNumber(RenderingChannels vertOutput);
//Gets the index of the given color output channel
//    (returns 1 for RC_COLOR, and 2-4 for RC_COLOR_OUT_[2-4]).
unsigned int GetColorOutputNumber(RenderingChannels colorOutput);