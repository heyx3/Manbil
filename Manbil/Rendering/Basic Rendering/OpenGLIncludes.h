#pragma once

#include <string>
#include <GLEW/glew.h>
#include <GLEW/wglew.h>
#include <SFML/OpenGL.hpp>


//This file contains various global types for easy interaction with OpenGL data types.


//A handle to some kind of OpenGL data (buffer, texture, shader program, etc).
typedef GLuint RenderObjHandle;
//The location of a uniform in a shader.
typedef GLint UniformLocation;

extern RenderObjHandle INVALID_RENDER_OBJ_HANDLE;
extern UniformLocation INVALID_UNIFORM_LOCATION;



//Grabs the next error on the render error queue.
const char* GetCurrentRenderingError(void);
//Removes all errors from the render error queue.
void ClearAllRenderingErrors(void);


enum PrimitiveTypes
{
	PT_POINTS,
	PT_LINE_LIST,
    PT_LINE_STRIP,
	PT_TRIANGLE_LIST,
    PT_TRIANGLE_STRIP,
};
//Converts the given primitive type to the corresponding GLenum.
GLenum PrimitiveTypeToGLEnum(PrimitiveTypes t);
//Converts the given primitive type to a Geometry Shader input keyword.
std::string PrimitiveTypeToGSInput(PrimitiveTypes t);
//Converts the given primitive type to a Geometry Shader output keyword.
std::string PrimitiveTypeToGSOutput(PrimitiveTypes t);
//Gets the number of vertices per a single instance of the given primitive.
unsigned int PrimitiveTypeToNVertices(PrimitiveTypes t);
//Converts the given primitive type into a descriptive string.
std::string PrimitiveTypeToString(PrimitiveTypes primType);
//Converts the result of "PrimitiveTypeToString" back into a PrimitiveTypes value.
//Returns PT_POINTS if the string isn't valid.
PrimitiveTypes PrimitiveTypeFromString(std::string primType);


enum TextureTypes
{
	TT_2D,
	TT_3D,
	TT_CUBE,
};
GLenum TextureTypeToGLEnum(TextureTypes t);

enum CubeTextureTypes
{
    CTT_X_NEG,
    CTT_X_POS,

    CTT_Y_NEG,
    CTT_Y_POS,

    CTT_Z_NEG,
    CTT_Z_POS,
};
GLenum TextureTypeToGLEnum(CubeTextureTypes t);


enum Shaders
{
    SH_VERTEX,
    SH_FRAGMENT,
    SH_GEOMETRY,
};
GLenum ShaderTypeToGLEnum(Shaders sh);
std::string ShaderTypeToString(Shaders sh);
//Returns SH_GEOMETRY if the given string couldn't be parsed.
Shaders ShaderTypeFromString(std::string sh);