#pragma once

#include "Math/Lower Math/Vectors.h"
#include "Rendering/Basic Rendering/RenderIOAttributes.h"


//Defines some simple, useful vertex structures.
//The general order for attributes is as follows:
// 1) Position
// 2) UV
// 3) Normal
// 4) Color
//The structs' attribute ordering is also described in their names.
//TODO: For each struct, add static getters like "int GetPosVertexInput()" for every attribute. Then change the various worlds to use them.


struct VertexPos
{
	Vector3f Pos;

	VertexPos(Vector3f pos = Vector3f()) : Pos(pos) { }

    static RenderIOAttributes GetAttributeData(void);
};

struct VertexPosColor
{
	Vector3f Pos;
	Vector3f Col;

    VertexPosColor(Vector3f pos = Vector3f(), Vector3f color = Vector3f(1.0f, 1.0f, 1.0f))
        : Pos(pos), Col(color) { }

    static RenderIOAttributes GetAttributeData(void);
};

struct VertexPosUV
{
	Vector3f Pos;
	Vector2f UV;

    VertexPosUV(Vector3f pos = Vector3f(), Vector2f uv = Vector2f())
        : Pos(pos), UV(uv) { }

    static RenderIOAttributes GetAttributeData(void);
};

struct VertexPosNormal
{
    Vector3f Pos, Normal;

    VertexPosNormal(Vector3f pos = Vector3f(), Vector3f normal = Vector3f(0.0f, 0.0f, 1.0f))
        : Pos(pos), Normal(normal) { }

    static RenderIOAttributes GetAttributeData(void);
};

struct VertexPosUVNormal
{
	Vector3f Pos;
	Vector2f UV;
	Vector3f Normal;

	VertexPosUVNormal(Vector3f pos = Vector3f(), Vector2f uv = Vector2f(),
                        Vector3f normal = Vector3f(0, 0, 1))
        : Pos(pos), UV(uv), Normal(normal) { }

    static RenderIOAttributes GetAttributeData(void);
};

struct VertexPosUVNormalColor
{
    Vector3f Pos;
    Vector2f UV;
    Vector3f Normal;
    Vector4f Color;

    VertexPosUVNormalColor(Vector3f pos = Vector3f(), Vector2f uv = Vector2f(),
                             Vector3f normal = Vector3f(0.0f, 0.0f, 1.0f),
                             Vector4f color = Vector4f(0.0f, 0.0f, 0.0f, 1.0f))
        : Pos(pos), UV(uv), Normal(normal), Color(color) { }

    static RenderIOAttributes GetAttributeData(void);
};