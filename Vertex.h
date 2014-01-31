#pragma once

#include "Math/Vectors.h"


//Represents a vertex with position, color, tex coord, and normal data.
struct Vertex
{
public:

	//Sets up the current OpenGL context to use this kind of vertex.
	static void EnableVertexAttributes(void);
	//Releases the current OpenGL context from using this kind of vertex.
	static void DisableVertexAttributes(void);



	Vector3f Position;
	Vector4f Color;
	Vector2f TexCoords;
	Vector3f Normal;

	Vertex(Vector3f position = Vector3f(0.0f, 0.0f, 0.0f))
		: Position(position), Color(1.0f, 1.0f, 1.0f, 1.0f), TexCoords(0.0f, 0.0f), Normal(0.0f, 0.0f, 1.0f) { }
	Vertex(Vector3f position, Vector2f texCoords)
		: Position(position), TexCoords(texCoords), Color(1.0f, 1.0f, 1.0f, 1.0f), Normal(0.0f, 0.0f, 1.0f) { }
	Vertex(Vector3f position, Vector4f color)
		: Position(position), Color(color), TexCoords(0.0f, 0.0f), Normal(0.0f, 0.0f, 1.0f) { }
	Vertex(Vector3f position, Vector2f texCoords, Vector3f normal)
		: Position(position), TexCoords(texCoords), Normal(normal), Color(1.0f, 1.0f, 1.0f, 1.0f) { }
	Vertex(Vector3f position, Vector2f texCoords, Vector4f color, Vector3f normal)
		: Position(position), Color(color), TexCoords(texCoords), Normal(normal) { }
};