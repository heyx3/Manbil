#pragma once

#include <string>
#include <unordered_map>

#include "Mesh.h"
#include "RenderInfo.h"
#include "Vertex.h"


//Represents a vertex and fragment shader combined.
//Refer to the document "Shader Definitions" for more info.
class Material
{
public:

	//The number of available 2d texture samplers, starting at u_sampler0.
	static const int TWODSAMPLERS = 5;



	//Initializes OpenGL drawing data that is global for all Material instances.
	static void InitializeMaterialDrawing(void);
	//Resets OpenGL drawing data that is global for all Material instances.
	static void EndMaterialDrawing(void);


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

	static const std::string wvpMatName, wMatName, vMatName, pMatName, samplerNames[TWODSAMPLERS], timeName;

	mutable std::string errorMsg;
	//Sets "errorMsg" to either an empty string or the current OpenGL error message, then returns whether there was an error message.
	bool CheckError(std::string errorStart) { errorMsg = GetCurrentRenderingError(); if (!errorMsg.empty()) errorMsg.insert(errorMsg.begin(), errorStart.begin(), errorStart.end()); return HasError(); }

	std::unordered_map<std::string, UniformLocation> uniforms;

	BufferObjHandle shaderProgram;
};