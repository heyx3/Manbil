#pragma once

#include <string>
#include <unordered_map>

#include "RenderInfo.h"
#include "RenderDataHandler.h"
#include "Vertex.h"

class Mesh;



//Represents a vertex and fragment shader combined.
//Refer to the document "Shader Definitions" for more info.
class Material
{
public:

    //The number of available 2d texture samplers, starting at u_sampler0.
    static const int TWODSAMPLERS = 5;


    //The 2d textures used for the sampler objects.
    BufferObjHandle TextureSamplers[TWODSAMPLERS];


    Material(std::string vertexShader, std::string pixelShader);
    //Creates a copy of the given material.
    Material(const Material & copy);

    //Releases this material, making it and all its copies invalid.
    //Using this material after it is deleted results in undefined behavior.
    void DeleteMaterial(void);


    //Error-handling.

    bool HasError(void) const { return !errorMsg.empty(); }
    std::string GetErrorMessage(void) const { return errorMsg; }
    void SetErrorMessage(std::string msg = "") { errorMsg = msg; }


    BufferObjHandle GetShaderProgram(void) const { return shaderProgram; }



    //Gets the location of the given vertex or fragment shader uniform for later writing.
    //Returns whether the uniform actually exists.
    bool AddUniform(std::string uniformName);


    //Uniform setters. Return "false" if the uniform wasn't found, or "true" otherwise.

    bool SetUniformF(std::string uniform, const float * start, int numbFloats);
    bool SetUniformI(std::string uniform, const int * start, int numbInts);
    bool SetUniformMat(std::string uniform, const Matrix4f & matrix);


    //Draws the given meshes, interpreting them as the given primitive type.
    bool Render(const RenderInfo & rendInfo, const std::vector<const Mesh*> & meshes);


    //Returns this material's shader program index.
    int GetHashCode(void) const { return shaderProgram; }


private:

    mutable std::string errorMsg;
    //Sets "errorMsg" to either an empty string or the current OpenGL error message, then returns whether there was an error message.
    bool CheckError(std::string errorStart) { errorMsg = GetCurrentRenderingError(); if (!errorMsg.empty()) errorMsg.insert(errorMsg.begin(), errorStart.begin(), errorStart.end()); return HasError(); }

    std::unordered_map<std::string, UniformLocation> uniforms;

    BufferObjHandle shaderProgram;
};


//Represents the combiniation of a vertex shader and a fragment shader.
struct RenderingPass
{
    std::string VertexShader, FragmentShader;
    RenderingPass(std::string vs, std::string fs) : VertexShader(vs), FragmentShader(fs) { }
};

//Represents all the texture samplers to be used for a rendering pass.
struct PassSamplers
{
public:
    BufferObjHandle Samplers[Material::TWODSAMPLERS];

    PassSamplers(BufferObjHandle samplers[Material::TWODSAMPLERS]) { for (int i = 0; i < Material::TWODSAMPLERS; ++i) Samplers[i] = samplers[i]; }
    PassSamplers(void) { for (int i = 0; i < Material::TWODSAMPLERS; ++i) Samplers[i] = 0; }

    BufferObjHandle & operator[](int index) { return Samplers[index]; }
    const BufferObjHandle & operator[](int index) const { return Samplers[index]; }
};



//TODO: Add u_cam_forward, u_cam_upward, and u_cam_side uniforms.

//Represents a vertex shader and multiple fragment shaders.
//Refer to the document "Shader Definitions" for more info.
class Material2
{
public:

    //The number of available 2d texture samplers, starting at u_sampler0.
    static const int TWODSAMPLERS = 5;

    //The names of the different uniforms.
    //TODO: Define.
    static const std::string WvpMatName, WorldMatName, ViewMatName, ProjMatName,
                             SamplerNames[TWODSAMPLERS], TimeName;



    //Creates a new material with the given vertex shaders and fragment shaders.
    //All rendering passes are done in the order they are given here.
    Material2(std::vector<RenderingPass> passes);
    Material2(const Material2 & cpy); //Don't implement; materials shouldn't be copied.
    ~Material2(void);


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
    bool SetTexture(BufferObjHandle texture, unsigned int sampler)
    {
        bool b = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
            b = SetTexture(i, texture, sampler) || b;
        return b;
    }
    //Sets the given default texture sampler for the given rendering pass.
    bool SetTexture(int pass, BufferObjHandle texture, unsigned int sampler)
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
    std::vector<std::unordered_map<std::string, UniformLocation>> uniforms;
    std::vector<BufferObjHandle> shaderPrograms;


    mutable std::string errorMsg;
    //Sets "errorMsg" to either an empty string or the current OpenGL error message, then returns whether there was an error message.
    bool CheckError(std::string errorStart) { errorMsg = GetCurrentRenderingError(); if (!errorMsg.empty()) errorMsg.insert(errorMsg.begin(), errorStart.begin(), errorStart.end()); return HasError(); }
};