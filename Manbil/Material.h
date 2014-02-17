#pragma once

#include <string>
#include <unordered_map>

#include "RenderInfo.h"
#include "RenderDataHandler.h"
#include "ShaderHandler.h"
#include "Vertex.h"
#include "Rendering/Materials/MaterialData.h"
#include "Rendering/Texture Management/TextureChannels.h"
#include "Rendering/Materials/UniformCollections.h"
#include "RenderTarget.h"


class Mesh;
typedef std::shared_ptr<sf::Texture> TexturePtr;



/*TODO: Finish rewriting.
    Render passes should be done with all world geometry at once, not one mesh at a time doing every pass! http://gamedev.stackexchange.com/questions/66945/how-many-rendering-passes-is-normal
    Post-Process Effects should be totally unrelated to the Material system -- they have a different purpose and have a wider array of inputs.
    For now, don't bother trying to work with/sort the transparent materials.
    Use the stuff in "UniformCollections.h" to handle the uniform system for Meshes. After the material is compiled, go through the uniform collection and set the uniform locations.
*/

//Represents some kind of surface to be drawn on.
class Mat
{
public:

    Mat(std::string & vShader, std::string & fShader, const UniformDictionary & uniforms,
        RenderingModes mode, bool isLit, LightSettings lightSettings);


    RenderingModes GetMode(void) const { return mode; }
    bool GetIsLit(void) const { return isLit; }
    const LightSettings & GetLightSettings(void) const { return lightSettings; }

    const std::vector<std::string> & GetUniforms(RenderPasses pass) const;


    //void Render(RenderPasses pass, RenderTarget * renderTargets, const RenderInfo & info, const std::vector<const Mesh*> & meshes);


private:

    std::unordered_map<std::string, UniformLocation> uniforms[(int)RenderPasses::Numb_Passes];

    void RenderBaseComponents(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    void RenderCombineComponents(const RenderInfo & info, const std::vector<const Mesh*> & meshes);
    void RenderApplyOcclusion(const RenderInfo & info, const std::vector<const Mesh*> & meshes);


    bool isLit;
    LightSettings lightSettings;
    RenderingModes mode;
};



/*

//Represents a vertex shader and multiple fragment shaders.
//Refer to the document "Shader Definitions" for more info.
class Material
{
public:

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


    //The following vectors are indexed by rendering pass.

    std::vector<PassSamplers> textureSamplers;
    std::vector<RenderingState> renderStates;
    std::vector<std::unordered_map<std::string, UniformLocation>> uniforms;
    std::vector<RenderObjHandle> shaderPrograms;


    mutable std::string errorMsg;
    //Sets "errorMsg" to either an empty string or the current OpenGL error message, then returns whether there was an error message.
    bool CheckError(std::string errorStart) { errorMsg = GetCurrentRenderingError(); if (!errorMsg.empty()) errorMsg.insert(errorMsg.begin(), errorStart.begin(), errorStart.end()); return HasError(); }
};
*/