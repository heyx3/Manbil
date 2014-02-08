#pragma once

#include <string>
#include <unordered_map>

#include "RenderInfo.h"
#include "RenderDataHandler.h"
#include "ShaderHandler.h"
#include "RenderingState.h"
#include "Vertex.h"

class Mesh;


//Constants used for the Material system.
struct MaterialConstants
{
public:
    //The number of available 2d texture samplers, starting at u_sampler0.
    static const int TWODSAMPLERS = 5;
};


//Represents the combiniation of a vertex shader and a fragment shader.
struct RenderingPass
{
    RenderingState RenderState;
    std::string VertexShader, FragmentShader;
    RenderingPass(std::string vs, std::string fs, RenderingState rendState = RenderingState()) : VertexShader(vs), FragmentShader(fs), RenderState(rendState) { }
};


//Represents all the texture samplers to be used for a rendering pass.
struct PassSamplers
{
public:
    RenderObjHandle Samplers[MaterialConstants::TWODSAMPLERS];
    Vector2f Panners[MaterialConstants::TWODSAMPLERS];
    Vector2f Scales[MaterialConstants::TWODSAMPLERS];
    Vector2f Offsets[MaterialConstants::TWODSAMPLERS];

    PassSamplers(RenderObjHandle samplers[MaterialConstants::TWODSAMPLERS] = 0);

    RenderObjHandle & operator[](int index) { return Samplers[index]; }
    const RenderObjHandle & operator[](int index) const { return Samplers[index]; }
};

typedef std::shared_ptr<sf::Texture> TexturePtr;

/*TODO: Time to rewrite the material system again!
    Instead of dealing with "uniforms", use "TextureChannelData", " and "ConstChannelData"
    Render passes should be done with all world geometry at once, not one mesh at a time doing every pass! Each pass should have a clear purpose as well -- http://gamedev.stackexchange.com/questions/66945/how-many-rendering-passes-is-normal
    Post-Process Effects should be totally unrelated to the Material system -- they have a different purpose and have more complext inputs.
    Refer to the "Rendering Hierarchy" document to see how rendering should be organized.
*/
enum RenderingMode
{
    RM_Opaque,
    RM_Transluscent,
    RM_Additive,
};

//Data for a channel, calculated by texture lookup.
struct TextureChannelData
{
public:
    Channels Channel;
    ChannelsIn InChannel;

    TexturePtr Value;
    Vector2f Offset, Scale, PanVelocity;
    TextureChannelData(Channels channel, TexturePtr value, ChannelsIn inChannel, Vector2f offset = Vector2f(), Vector2f scale = Vector2f(1.0f, 1.0f), Vector2f panVelocity = Vector2f(0.0f, 0.0f))
        : Channel(channel), InChannel(inChannel), Value(value), Offset(offset), Scale(scale), PanVelocity(panVelocity)
    { }
};
enum Channels
{
    Diffuse = 0,
    Opacity,

    Normal,
    Bump,
    Specular,

    DiffuseIntensity,
    SpecularIntensity,

    NUMB_CHANNELS,
};
//Data for a channel, calculated by a constant float uniform.
struct FloatChannelData
{
public:
    Channels Channel;
    float Values[4];
    int NumbValues;
    FloatChannelData(Channels channel, float value) : Channel(channel), NumbValues(1) { Values[0] = value; }
    FloatChannelData(Channels channel, Vector2f value) : Channel(channel), NumbValues(2) { Values[0] = value.x; Values[1] = value.y; }
    FloatChannelData(Channels channel, Vector3f value) : Channel(channel), NumbValues(3) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; }
    FloatChannelData(Channels channel, Vector4f value) : Channel(channel), NumbValues(4) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; Values[3] = value.w; }
    FloatChannelData(Channels channel, float * vals, int nValues) : Channel(channel), NumbValues(nValues) { for (int i = 0; i < NumbValues; ++i) Values[i] = vals[i]; }
};
//Data for a channel, calculated by a constant float uniform.
struct IntChannelData
{
public:
    Channels Channel;
    int Values[4];
    int NumbValues;
    IntChannelData(Channels channel, int value) : Channel(channel), NumbValues(1) { Values[0] = value; }
    IntChannelData(Channels channel, Vector2i value) : Channel(channel), NumbValues(2) { Values[0] = value.x; Values[1] = value.y; }
    IntChannelData(Channels channel, Vector3i value) : Channel(channel), NumbValues(3) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; }
    IntChannelData(Channels channel, Vector4i value) : Channel(channel), NumbValues(4) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; Values[3] = value.w; }
    IntChannelData(Channels channel, int * vals, int nValues) : Channel(channel), NumbValues(nValues) { for (int i = 0; i < NumbValues; ++i) Values[i] = vals[i]; }
};

class Mat
{
public:

    RenderingMode GetMode(void) const { return mode; }


private:

    RenderingMode mode;
};


//Represents a vertex shader and multiple fragment shaders.
//Refer to the document "Shader Definitions" for more info.
class Material
{
public:

    //The different basic data channels making up a material. These may be textures or uniforms.
    enum Channels
    {
        Color = 0,
        Opacity,

        Normal,
        Bump,
        Specular,

        Special1,
        Special2,
        Special3,
        Special4,
        Special5,
        Special6,

        NUMB_CHANNELS,
    };
    //Data for a channel, calculated by texture lookup.
    struct TextureChannelData
    {
    public:
        Channels Channel;
        ChannelsIn InChannel;

        TexturePtr Value;
        Vector2f Offset, Scale, PanVelocity;
        TextureChannelData(Channels channel, TexturePtr value, ChannelsIn inChannel, Vector2f offset = Vector2f(), Vector2f scale = Vector2f(1.0f, 1.0f), Vector2f panVelocity = Vector2f(0.0f, 0.0f))
            : Channel(channel), InChannel(inChannel), Value(value), Offset(offset), Scale(scale), PanVelocity(panVelocity) { }
    };
    //Data for a channel, calculated by a constant float uniform.
    struct FloatChannelData
    {
    public:
        Channels Channel;
        float Values[4];
        int NumbValues;
        FloatChannelData(Channels channel, float value) : Channel(channel), NumbValues(1) { Values[0] = value; }
        FloatChannelData(Channels channel, Vector2f value) : Channel(channel), NumbValues(2) { Values[0] = value.x; Values[1] = value.y; }
        FloatChannelData(Channels channel, Vector3f value) : Channel(channel), NumbValues(3) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; }
        FloatChannelData(Channels channel, Vector4f value) : Channel(channel), NumbValues(4) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; Values[3] = value.w; }
        FloatChannelData(Channels channel, float * vals, int nValues) : Channel(channel), NumbValues(nValues) { for (int i = 0; i < NumbValues; ++i) Values[i] = vals[i]; }
    };
    //Data for a channel, calculated by a constant float uniform.
    struct IntChannelData
    {
    public:
        Channels Channel;
        int Values[4];
        int NumbValues;
        IntChannelData(Channels channel, int value) : Channel(channel), NumbValues(1) { Values[0] = value; }
        IntChannelData(Channels channel, Vector2i value) : Channel(channel), NumbValues(2) { Values[0] = value.x; Values[1] = value.y; }
        IntChannelData(Channels channel, Vector3i value) : Channel(channel), NumbValues(3) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; }
        IntChannelData(Channels channel, Vector4i value) : Channel(channel), NumbValues(4) { Values[0] = value.x; Values[1] = value.y; Values[2] = value.z; Values[3] = value.w; }
        IntChannelData(Channels channel, int * vals, int nValues) : Channel(channel), NumbValues(nValues) { for (int i = 0; i < NumbValues; ++i) Values[i] = vals[i]; }
    };

    bool IsChannelEnabled(Channels channel) { return channelsEnabled[(int)channel]; }
    bool IsChannelTexture(Channels channel) {  }



    //The names of the different uniforms.
    //TODO: Define.
    static const std::string WvpMatName, WorldMatName, ViewMatName, ProjMatName,
                             TextureScaleName, TimeName,
                             SamplerNames[MaterialConstants::TWODSAMPLERS],
                             SamplerScales[MaterialConstants::TWODSAMPLERS],
                             SamplerPans[MaterialConstants::TWODSAMPLERS];



    //Creates a new material with the given vertex shaders and fragment shaders.
    //All rendering passes are done in the order they are given here.
    Material(const std::vector<RenderingPass> & passes);
    //Creates a material that has only one pass.
    Material(const RenderingPass & shaders);
    Material(const Material & cpy); //Don't implement; materials shouldn't be copied.
    ~Material(void);


    //Error-handling.

    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetErrorMessage(void) const { return errorMsg; }
    void SetErrorMessage(std::string msg = "") { errorMsg = msg; }

    //Gets the number of passes this material uses.
    int GetNumbPasses(void) const { return shaderPrograms.size(); }

    //Gets the different shader programs in the order they are rendered.
    const std::vector<RenderObjHandle> & GetShaderPasses(void) const { return shaderPrograms; }
    //Gets the different rendering states for each pass in the order the passes are rendered in.
    const std::vector<RenderingState> & GetPassRenderStates(void) const { return renderStates; }
    //Gets the different texture samplers for each pass in the order the passes are rendered in.
    const std::vector<PassSamplers> & GetPassSamplers(void) const { return textureSamplers; }

    //Gets the different shader programs in the order they are rendered.
    std::vector<RenderObjHandle> & GetShaderPasses(void) { return shaderPrograms; }
    //Gets the different rendering states for each pass in the order the passes are rendered in.
    std::vector<RenderingState> & GetPassRenderStates(void) { return renderStates; }
    //Gets the different texture samplers for each pass in the order the passes are rendered in.
    std::vector<PassSamplers> & GetPassSamplers(void) { return textureSamplers; }

    //Returns this material's first shader program index.
    int GetHashCode(void) const { if (shaderPrograms.size() == 0) return 1; return shaderPrograms[0]; }

    //Returns whether or not the uniform was successfully added.
    //The attempt will be unsuccessful if:
    //  a) the uniform was already found before.
    //  b) the uniform doesn't exist in any of this material's passes.
    bool AddUniform(std::string uniform)
    {
        bool exists = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
            exists = TryAddUniform(i, uniform) || exists;
        return exists;
    }
    //Returns whether or not this material has the location of the given uniform.
    bool HasUniform(std::string uniform)
    {
        bool exists = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
            exists = HasUniform(uniform, i) || exists;
        return exists;
    }
    //Returns whether or not this material has the location of the given uniform for the given pass.
    bool HasUniform(std::string uniform, unsigned int pass)
    {
        assert(pass < GetNumbPasses());
        return uniforms[pass].find(uniform) != uniforms[pass].end();
    }

    //Sets the given default texture sampler for all passes.
    bool SetTexture(unsigned int sampler, RenderObjHandle texture, Vector2f scale = Vector2f(1.0f, 1.0f), Vector2f panner = Vector2f())
    {
        bool b = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
            b = SetTexture(i, sampler, texture, scale, panner) || b;
        return b;
    }
    //Sets the given default texture sampler for the given rendering pass.
    bool SetTexture(int pass, unsigned int sampler, RenderObjHandle texture, Vector2f scale = Vector2f(1.0f, 1.0f), Vector2f panner = Vector2f())
    {
        assert(pass < GetNumbPasses());
        textureSamplers[pass].Samplers[sampler] = texture;
        textureSamplers[pass].Panners[sampler] = panner;
        textureSamplers[pass].Scales[sampler] = scale;
        return true;
    }

    //Renders all passes. Returns whether all passes were successful.
    bool Render(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    //Returns whether the render was successful.
    bool Render(unsigned int pass, const RenderInfo & info, const std::vector<const Mesh*> & meshes);

private:

    //Renders the given mesh for the given pass. Assume uniforms and textures are already all set up.
    bool Render(const Mesh * mesh, const RenderInfo & info, unsigned int pass);

    bool TryAddUniform(unsigned int programIndex, std::string uniform);

    bool SetUniformF(std::string uniform, const float * data, int nData)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
        {
            ShaderHandler::UseShader(shaderPrograms[i]);
            tried = TrySetUniformF(i, uniform, data, nData) || tried;
        }
        return tried;
    }
    bool SetUniformArrayF(std::string uniform, const float * dataSplit, int floatsPerUniform, int nUniforms)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
        {
            ShaderHandler::UseShader(shaderPrograms[i]);
            tried = TrySetUniformArrayF(i, uniform, dataSplit, nUniforms, floatsPerUniform) || tried;
        }
        return tried;
    }
    bool SetUniformArrayF(std::string uniform, int arrayElement, const float * uniformData, int nData)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
        {
            ShaderHandler::UseShader(shaderPrograms[i]);
            tried = TrySetUniformArrayF(i, uniform, arrayElement, uniformData, nData) || tried;
        }
        return tried;
    }
    bool SetUniformI(std::string uniform, const int * data, int nData)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
        {
            ShaderHandler::UseShader(shaderPrograms[i]);
            tried = TrySetUniformI(i, uniform, data, nData) || tried;
        }
        return tried;
    }
    bool SetUniformArrayI(std::string uniform, const int * dataSplit, int intsPerUniform, int nUniforms)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
        {
            ShaderHandler::UseShader(shaderPrograms[i]);
            tried = TrySetUniformArrayI(i, uniform, dataSplit, nUniforms, intsPerUniform) || tried;
        }
        return tried;
    }
    bool SetUniformArrayI(std::string uniform, int arrayElement, const int * uniformData, int nData)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
        {
            ShaderHandler::UseShader(shaderPrograms[i]);
            tried = TrySetUniformArrayI(i, uniform, arrayElement, uniformData, nData) || tried;
        }
        return tried;
    }
    bool SetUniformMat(std::string uniform, const Matrix4f & data)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
        {
            ShaderHandler::UseShader(shaderPrograms[i]);
            tried = TrySetUniformMat(i, uniform, data) || tried;
        }
        return tried;
    }

    bool TrySetUniformF(unsigned int programIndex, std::string uniform, const float * data, int nData)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformValue(uniforms[programIndex][uniform], nData, data);
        return true;
    }
    bool TrySetUniformArrayF(unsigned int programIndex, std::string uniform, const float * data, unsigned int nUniforms, unsigned int nFloatsPerUniform)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformArrayValue(uniforms[programIndex][uniform], nUniforms, nFloatsPerUniform, data);
        return true;
    }
    bool TrySetUniformArrayF(unsigned int programIndex, std::string uniform, unsigned int arrayElement, const float * data, int nData)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformValue(uniforms[programIndex][uniform] + arrayElement, nData, data);
    }
    bool TrySetUniformI(unsigned int programIndex, std::string uniform, const int * data, int nData)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformValue(uniforms[programIndex][uniform], nData, data);
        return true;
    }
    bool TrySetUniformArrayI(unsigned int programIndex, std::string uniform, const int * data, unsigned int nUniforms, unsigned int nIntsPerUniform)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformArrayValue(uniforms[programIndex][uniform], nUniforms, nIntsPerUniform, data);
        return true;
    }
    bool TrySetUniformArrayI(unsigned int programIndex, std::string uniform, int arrayElement, const int * data, int nData)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformValue(uniforms[programIndex][uniform] + arrayElement, nData, data);
    }
    bool TrySetUniformMat(unsigned int programIndex, std::string uniform, const Matrix4f & data)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetMatrixValue(uniforms[programIndex][uniform], data);
        return true;
    }


    //Indexed by channel.
    bool channelsEnabled[(int)Channels::NUMB_CHANNELS];


    //The following vectors are indexed by rendering pass.

    std::vector<PassSamplers> textureSamplers;
    std::vector<RenderingState> renderStates;
    std::vector<std::unordered_map<std::string, UniformLocation>> uniforms;
    std::vector<RenderObjHandle> shaderPrograms;


    mutable std::string errorMsg;
    //Sets "errorMsg" to either an empty string or the current OpenGL error message, then returns whether there was an error message.
    bool CheckError(std::string errorStart) { errorMsg = GetCurrentRenderingError(); if (!errorMsg.empty()) errorMsg.insert(errorMsg.begin(), errorStart.begin(), errorStart.end()); return HasError(); }
};