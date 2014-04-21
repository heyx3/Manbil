#pragma once



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
enum RenderingChannels
{
    //Screen-space position of vertex.
    RC_ScreenVertexPosition,

    //Base color of surface.
    RC_Color,

    //Transparency of surface.
    RC_Opacity,

    //An "invalid" vertex output (interpolated fragment input).
    RC_VERTEX_OUT_INVALID,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_1,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_2,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_3,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_4,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_5,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_6,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_7,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_8,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_9,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_10,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_11,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_12,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_13,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_14,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_15,
    //Vertex output (interpolated fragment input).
    RC_VERTEX_OUT_16,
};

//Gets whether the given channel is a vertex output.
//"includeInvalidOutput" indicates what to return if the value is RC_VERTEX_OUT_INVALID.
bool IsChannelVertexOutput(RenderingChannels channel, bool includeInvalidOutput);
//Gets whether the given channel is used in the given rendering mode with the given material light settings.
bool IsChannelUsed(RenderingChannels channel, RenderingModes mode, LightSettings settings, bool isLit);
//Gets the index of the given vertex output channel (returns 0 for RC_VERTEX_OUT_INVALID).
unsigned int GetVertexOutputNumber(RenderingChannels vertOutput);