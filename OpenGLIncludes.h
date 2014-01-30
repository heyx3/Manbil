#pragma once


#include "GL/glew.h"
#include "GL/wglew.h"
#include "SFML/OpenGL.hpp"


typedef GLuint BufferObjHandle;
typedef GLint UniformLocation;
typedef GLchar Char;
typedef GLvoid Void;



//Grabs the next error on the render error queue.
const char * GetCurrentRenderingError(void);
//Removes all errors from the render error queue.
void ClearAllRenderingErrors(void);


enum PrimitiveTypes
{
	Points,
	Triangles,
	Lines,
};
GLenum PrimitiveTypeToGLEnum(PrimitiveTypes t);


enum TextureTypes
{
	Tex_OneD,
	Tex_TwoD,
	Tex_ThreeD,
	Tex_Cube,
};
GLenum TextureTypeToGLEnum(TextureTypes t);


//Useful data about vertices/indices of a mesh.
struct VertexIndexData
{
public:

	//Creates a VertexIndexData with invalid data.
	VertexIndexData(void) : indices(-1), vertices(-1), hVertex(0), hIndex(0), firstVertex(0) { }
	//Creates a VertexIndexData that doesn't use indices.
	VertexIndexData(int nVertices, BufferObjHandle vbo, int _firstVertex = 0) : indices(-1), vertices(nVertices), hVertex(vbo), hIndex(0), firstVertex(_firstVertex) { }
	//Creates a VertexIndexData that uses indices.
	VertexIndexData(int nVertices, BufferObjHandle vbo, int nIndices, BufferObjHandle ibo) : indices(nIndices), vertices(nVertices), hVertex(vbo), hIndex(ibo), firstVertex(0) { }
	//Creates a copy of the VertexIndexData with a different starting index.
	VertexIndexData(const VertexIndexData & copy, int newFirstIndex = 0) : indices(copy.indices), vertices(copy.vertices), hVertex(copy.hVertex), hIndex(copy.hIndex), firstVertex(newFirstIndex) { }

	BufferObjHandle GetVerticesHandle(void) const { return hVertex; }
	BufferObjHandle GetIndicesHandle(void) const { return hIndex; }

	int GetIndicesCount(void) const { return indices; }
	int GetVerticesCount(void) const { return vertices; }

	int GetFirstVertex(void) const { return firstVertex; }

	bool IsValid(void) const { return (vertices != -1); }
	bool UsesIndices(void) const { return (indices != -1); }

private:

	int indices, vertices, firstVertex;
	BufferObjHandle hVertex, hIndex;
};