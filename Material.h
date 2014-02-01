#pragma once

#include <string>
#include <unordered_map>

#include "Mesh.h"
#include "RenderInfo.h"
#include "RenderDataHandler.h"
#include "Vertex.h"


//Represents the combiniation of a vertex shader and a fragment shader.
struct MaterialShaders
{
    std::string VertexShader, FragmentShader;
    MaterialShaders(std::string vs, std::string fs) : VertexShader(vs), FragmentShader(fs) { }
};



//Represents a vertex shader and multiple fragment shaders.
//Refer to the document "Shader Definitions" for more info.
class Material2
{
public:

    //The number of available 2d texture samplers, starting at u_sampler0.
    static const int TWODSAMPLERS = 5;

    //The names of the different uniforms.
    static const std::string WvpMatName, WMatName, VMatName, PMatName, SamplerNames[TWODSAMPLERS], TimeName;


    //The 2d textures used for the sampler objects.
    BufferObjHandle TextureSamplers[TWODSAMPLERS];

    //Creates a new material with the given vertex shaders and fragment shaders.
    //All rendering passes are done in the order they are given here.
    Material2(std::vector<MaterialShaders> passes);
    //Creates a brand new material with the same shader code and uniforms as this one.
    Material2(const Material2 & cpy);
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
    bool SetUniformF(std::string uniform, float * data, int nData)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
            tried = TrySetUniformF(i, uniform, data, nData) || tried;
        return tried;
    }
    //Returns whether or not the uniform exists in any of the passes.
    bool SetUniformI(std::string uniform, int * data, int nData)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
            tried = TrySetUniformI(i, uniform, data, nData) || tried;
        return tried;
    }
    //Returns whether or not the uniform exists in any of the passes.
    bool SetUniformMat(std::string uniform, const Matrix4f & data)
    {
        bool tried = false;
        for (int i = 0; i < GetNumbPasses(); ++i)
            tried = TrySetUniformMat(i, uniform, data) || tried;
        return tried;
    }

    //Returns whether the render was successful.
    bool Render(int pass, const RenderInfo & info, const std::vector<Mesh*> meshes);

private:

    bool TryAddUniform(int programIndex, std::string uniform);

    bool TrySetUniformF(int programIndex, std::string uniform, float * data, int nData)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformValue(uniforms[programIndex][uniform], nData, data);
        return true;
    }
    bool TrySetUniformI(int programIndex, std::string uniform, int * data, int nData)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetUniformValue(uniforms[programIndex][uniform], nData, data);
        return true;
    }
    bool TrySetUniformMat(int programIndex, std::string uniform, const Matrix4f & data)
    {
        if (uniforms[programIndex].find(uniform) == uniforms[programIndex].end()) return false;
        RenderDataHandler::SetMatrixValue(uniforms[programIndex][uniform], data);
        return true;
    }


    std::vector<std::unordered_map<std::string, UniformLocation>> uniforms;
    std::vector<BufferObjHandle> shaderPrograms;

    mutable std::string errorMsg;
    //Sets "errorMsg" to either an empty string or the current OpenGL error message, then returns whether there was an error message.
    bool CheckError(std::string errorStart) { errorMsg = GetCurrentRenderingError(); if (!errorMsg.empty()) errorMsg.insert(errorMsg.begin(), errorStart.begin(), errorStart.end()); return HasError(); }
};


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