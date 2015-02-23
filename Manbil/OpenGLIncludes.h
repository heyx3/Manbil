#pragma once

#include <string>
#include "GL/glew.h"
#include "GL/wglew.h"
#include "SFML/OpenGL.hpp"


//This file contains various global types for easy interaction with OpenGL data types.


//An OpenGL pointer to some kind of data (buffer, texture, shader program, etc).
typedef GLuint RenderObjHandle;
//An OpenGL pointer to the location of a uniform in a shader.
typedef GLint UniformLocation;

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


enum TextureTypes
{
	TT_1D,
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