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

    PassSamplers(RenderObjHandle samplers[MaterialConstants::TWODSAMPLERS]) { for (int i = 0; i < MaterialConstants::TWODSAMPLERS; ++i) Samplers[i] = samplers[i]; }
    PassSamplers(void) { for (int i = 0; i < MaterialConstants::TWODSAMPLERS; ++i) Samplers[i] = 0; }

    RenderObjHandle & operator[](int index) { return Samplers[index]; }
    const RenderObjHandle & operator[](int index) const { return Samplers[index]; }
};


//TODO: Add u_cam_forward, u_cam_upward, and u_cam_side uniforms.

//Represents a vertex shader and multiple fragment shaders.
//Refer to the document "Shader Definitions" for more info.
class Material
{
public:

    //The names of the different uniforms.
    //TODO: Define.
    static const std::string WvpMatName, WorldMatName, ViewMatName, ProjMatName,
        SamplerNames[MaterialConstants::TWODSAMPLERS], TimeName;



    //Creates a new material with the given vertex shaders and fragment shaders.
    //All rendering passes are done in the order they are given here.
    Material(std::vector<RenderingPass> passes);
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

    //Returns whether or not the uniform exists in any of the passes.
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
    //Returns whether or not the uniform exists in any of the passes.
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
    //Returns whether or not the uniform exists in any of the passes.
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

    //Sets the given default texture sampler for all passes.
    bool SetTexture(RenderObjHandle texture, unsigned int sampler)
    {
        bool b = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
            b = SetTexture(i, texture, sampler) || b;
        return b;
    }
    //Sets the given default texture sampler for the given rendering pass.
    bool SetTexture(int pass, RenderObjHandle texture, unsigned int sampler)
    {
        assert(pass < GetNumbPasses());
        textureSamplers[pass].Samplers[sampler] = texture;
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

    bool TrySetUniformF(unsigned int programIndex, std::string uniform, const float * data, int nData)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformValue(uniforms[programIndex][uniform], nData, data);
        return true;
    }
    bool TrySetUniformI(unsigned int programIndex, std::string uniform, const int * data, int nData)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformValue(uniforms[programIndex][uniform], nData, data);
        return true;
    }
    bool TrySetUniformMat(unsigned int programIndex, std::string uniform, const Matrix4f & data)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetMatrixValue(uniforms[programIndex][uniform], data);
        return true;
    }


    //The following vectors are indexed by rendering pass.

    std::vector<PassSamplers> textureSamplers;
    std::vector<RenderingState> renderStates;
    std::vector<std::unordered_map<std::string, UniformLocation>> uniforms;
    std::vector<RenderObjHandle> shaderPrograms;


    mutable std::string errorMsg;
    //Sets "errorMsg" to either an empty string or the current OpenGL error message, then returns whether there was an error message.
    bool CheckError(std::string errorStart) { errorMsg = GetCurrentRenderingError(); if (!errorMsg.empty()) errorMsg.insert(errorMsg.begin(), errorStart.begin(), errorStart.end()); return HasError(); }
};