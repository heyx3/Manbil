#pragma once

#include <string>
#include "GL/glew.h"
#include "GL/wglew.h"
#include "SFML/OpenGL.hpp"


//An OpenGL pointer to some kind of data (buffer, texture, etc).
typedef GLuint RenderObjHandle;
//An OpenGL pointer to the location of a uniform in a shader.
typedef GLint UniformLocation;

typedef GLchar Char;
typedef GLvoid Void;


//TODO: Use this for all classes that should use error message handling.
#define ERROR_MESSAGE_HANDLING \
    protected: \
        std::string errorMsg; \
        bool SetErrorMessageIfError(std::string prefix = "") { errorMsg = GetCurrentRenderingError(); if (!errorMsg.empty()) { errorMsg = prefix + errorMsg; return false; } return true; } \
    public: \
        std::string GetErrorMessage(void) const { return errorMsg; } \
        bool HasError(void) const { return !errorMsg.empty(); } \
        



//Grabs the next error on the render error queue.
const char * GetCurrentRenderingError(void);
//Removes all errors from the render error queue.
void ClearAllRenderingErrors(void);


enum PrimitiveTypes
{
	Points,
	LineList,
    LineStrip,
	TriangleList,
    TriangleStrip,
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


//Useful data about vertices/indices of a mesh.
struct VertexIndexData
{
public:

	//Creates a VertexIndexData with invalid data.
	VertexIndexData(void) : indices(-1), vertices(-1), hVertex(0), hIndex(0), firstVertex(0) { }
	//Creates a VertexIndexData that doesn't use indices.
	VertexIndexData(int nVertices, RenderObjHandle vbo, int _firstVertex = 0) : indices(-1), vertices(nVertices), hVertex(vbo), hIndex(0), firstVertex(_firstVertex) { }
	//Creates a VertexIndexData that uses indices.
	VertexIndexData(int nVertices, RenderObjHandle vbo, int nIndices, RenderObjHandle ibo) : indices(nIndices), vertices(nVertices), hVertex(vbo), hIndex(ibo), firstVertex(0) { }
	//Creates a copy of the VertexIndexData with a different starting index.
	VertexIndexData(const VertexIndexData & copy, int newFirstIndex = 0) : indices(copy.indices), vertices(copy.vertices), hVertex(copy.hVertex), hIndex(copy.hIndex), firstVertex(newFirstIndex) { }

	RenderObjHandle GetVerticesHandle(void) const { return hVertex; }
	RenderObjHandle GetIndicesHandle(void) const { return hIndex; }

	int GetIndicesCount(void) const { return indices; }
	int GetVerticesCount(void) const { return vertices; }

	int GetFirstVertex(void) const { return firstVertex; }

	bool IsValid(void) const { return (vertices != -1); }
	bool UsesIndices(void) const { return (indices != -1); }

private:

	int indices, vertices, firstVertex;
	RenderObjHandle hVertex, hIndex;
};