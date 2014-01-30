#pragma once

#include "Vectors.h"
#include "OpenGLIncludes.h"


struct VertexPos
{
	Vector3f Pos;
	VertexPos(Vector3f pos = Vector3f()) : Pos(pos) { }

	static void EnableAttributes(void);
	static void DisableAttributes(void);
};

struct VertexPosColor
{
	Vector3f Pos;
	Vector3f Col;

	VertexPosColor(Vector3f pos = Vector3f(), Vector3f color = Vector3f(1.0f, 1.0f, 1.0f)) : Pos(pos), Col(color) { }

	static void EnableAttributes(void);
	static void DisableAttributes(void);
};

struct VertexPosTex1
{
	Vector3f Pos;
	Vector2f TexCoords;
	VertexPosTex1(Vector3f pos = Vector3f(), Vector2f texCoords = Vector2f()) : Pos(pos), TexCoords(texCoords) { }

	static void EnableAttributes(void);
	static void DisableAttributes(void);
};

struct VertexPosTex2
{
	Vector3f Pos;
	Vector2f TexCoords1, TexCoords2;
	VertexPosTex2(Vector3f pos = Vector3f(), Vector2f texCoords1 = Vector2f(), Vector2f texCoords2 = Vector2f()) : Pos(pos), TexCoords1(texCoords1), TexCoords2(texCoords2) { }

	static void EnableAttributes(void);
	static void DisableAttributes(void);
};

struct VertexPosTex1Normal
{
	Vector3f Pos;
	Vector2f TexCoords;
	Vector3f Normal;
	VertexPosTex1Normal(Vector3f pos = Vector3f(), Vector2f texCoords = Vector2f(), Vector3f normal = Vector3f(0, 0, 1))
		: Pos(pos), TexCoords(texCoords), Normal(normal) { }

	static void EnableAttributes(void);
	static void DisableAttributes(void);
};