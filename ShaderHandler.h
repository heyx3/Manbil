#pragma once

#include <string>
#include "OpenGLIncludes.h"
#include "SFML/Graphics/Image.hpp"


//Handles initialization/manipulation of OpenGL shaders.
//Functions that return a "bool" return whether or not the function was a success.
//   If it wasn't, the specific error can be found with "GetErrorMessage()".
class ShaderHandler
{
public:

	typedef std::vector<RenderObjHandle> ShaderObjectList;


	//The size of the error message string.
	static const int ERROR_MESSAGE_SIZE = 1024;


	//Returns a string representing the most recent error to occur in the use of this class.
	static const char* const GetErrorMessage(void) { return errorMsg; }
	//Clears the recorded error from this class.
	static void ClearErrorMessage(void) { errorMsg[0] = '\0'; }


	//Returns whether or not the shader program was created successfully.
	static bool CreateShaderProgram(RenderObjHandle & out_handle);
	//Returns whether or not the creation was successful.
	static bool CreateShader(RenderObjHandle shaderProgram, RenderObjHandle& out_handle, const Char* shaderText, GLenum shaderType);
	//Returns whether or not the process was successful. "validate" does an extra check of shader validity (useful when debugging).
	static bool FinalizeShaders(RenderObjHandle shaderProgram, bool validate = true);


	//Deletes the given vector of shader objects. Can be done after finalizing their shader program to save space.
	static void DeleteShaders(ShaderObjectList * toDelete) { for (ShaderObjectList::iterator it = toDelete->begin(); it != toDelete->end(); ++it) glDeleteProgram(*it); toDelete->clear(); }


	//Sets the given shader program to be used.
	static void UseShader(RenderObjHandle shaderProgram) { glUseProgram(shaderProgram); }


	//Draws vertices using the given primitive type and the given number of vertices. The third optional parameter is the first vertex to draw.
	static void DrawVertices(PrimitiveTypes pType, int nVertices, int firstVertex = 0) { glDrawArrays(PrimitiveTypeToGLEnum(pType), firstVertex, nVertices); }
	//Draws indexed vertices using the given primitive type and vertex/index information. If 0 is passed for "indicesLocation", the index buffer will be used instead.
	static void DrawIndexedVertices(PrimitiveTypes pType, int nIndices, Void* indicesLocation = (Void*)0) { glDrawElements(PrimitiveTypeToGLEnum(pType), nIndices, GL_UNSIGNED_INT, indicesLocation); }

private:

	static char errorMsg[ERROR_MESSAGE_SIZE];
	static void SetErrorMsg(const char * error) { strncpy(errorMsg, error, ERROR_MESSAGE_SIZE); }
};