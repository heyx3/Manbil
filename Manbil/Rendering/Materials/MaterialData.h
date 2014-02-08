#pragma once

#include <string>
#include "../../RenderingState.h"
#include "RenderingModes.h"



//Constants/information used for the Material system.
struct MaterialConstants
{
public:
    static RenderingState GetRenderingState(RenderingModes mode);
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

    Numb_Passes,
};


struct ChannelData
{
public:

    struct UniformDeclaration
    {
    public:
        std::string Type, Name;
        UniformDeclaration(std::string type, std::string name) :Type(type), Name(name) { }
    };
    struct FunctionDeclaration
    {
    public:
        std::string Name, ReturnValue, Arguments, Body;
        FunctionDeclaration(std::string name, std::string returnVal, std::string params, std::string body)
            : Name(name), ReturnValue(returnVal), Arguments(params), Body(body) { }
    };
    struct Texture2DDeclaration
    {
    public:
        std::string Name;
        Texture
    };


    RenderingChannels Channel;

    std::vector<UniformDeclaration> Declarations;

};

//Data for a channel, computed using some shader code.
//"Header" is the shader code that declares associated uniforms/functions.
//"ChannelOutput" is an expression (to be inserted into "main()") that evaluates to the channel output value.
struct ComputedChannelData
{
public:

    struct UniformDeclaration
    {
    public:
        std::string Type, Name;
        UniformDeclaration(std::string type, std::string name) : Type(type), Name(name) { }
    };

    RenderingChannels Channel;
    //The shader code that declares any helper functions.
    std::string Header;
    //A GLSL expression that evaluates to the channel output.
    std::string ChannelOutput;
    //The uniforms needed to create the channel data.
    std::vector<UniformDeclaration> Uniforms;

    ComputedChannelData(RenderingChannels channel, std::string header, std::string channelOutput, std::vector<UniformDeclaration> uniforms)
        : Channel(channel), Header(header), ChannelOutput(channelOutput), Uniforms(uniforms) { }
};

//Data for a channel, calculated by texture lookup.
struct TextureChannelData
{
public:
    RenderingChannels Channel;
    ChannelsIn ChannelReader;
    TexturePtr Value;
    Vector2f Offset, Scale, PanVelocity;
    TextureChannelData(RenderingChannels channel, ChannelsIn reader, TexturePtr value,
                       Vector2f offset = Vector2f(), Vector2f scale = Vector2f(1.0f, 1.0f),
                       Vector2f panVelocity = Vector2f(0.0f, 0.0f))
                       : Channel(channel), ChannelReader(reader), Value(value), Offset(offset), Scale(scale), PanVelocity(panVelocity){ }
};

//Data for a channel, calculated by a constant float/vec2/vec3/vec4 uniform.
struct UniformChannelData
{
public:
    RenderingChannels Channel;
    float Values[4];
    int NumbValues;
    UniformChannelData(RenderingChannels channel, float value) : Channel(channel), NumbValues(1) { Values[0] = value; }
    UniformChannelData(RenderingChannels channel, Vector2f value) : Channel(channel), NumbValues(2) { Values[0] = value.x; Values[1] = value.y; }
    UniformChannelData(RenderingChannels channel, Vector3f value) : Channel(channel), NumbValues(3) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; }
    UniformChannelData(RenderingChannels channel, Vector4f value) : Channel(channel), NumbValues(4) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; Values[3] = value.w; }
    UniformChannelData(RenderingChannels channel, float * vals, int nValues) : Channel(channel), NumbValues(nValues) { for (int i = 0; i < NumbValues; ++i) Values[i] = vals[i]; }
};