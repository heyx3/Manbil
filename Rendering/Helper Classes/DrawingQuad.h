#pragma once

#include "../../OpenGLIncludes.h"
#include "../../Mesh.h"
#include "../../Material.h"
#include "../../Vertex.h"

//Represents a simple 1x1 square.
class DrawingQuad
{
public:

    Mesh & GetMesh(void) { return quad; }
    const Mesh & GetMesh(void) const { return quad; }

    void SetQuadPos(Vector2f pos) { quad.Transform.SetPosition(Vector3f(pos.x, pos.y, 0.0f)); }
    void SetQuadSize(Vector2f size) { quad.Transform.SetScale(Vector3f(size.x, size.y, 1.0f)); }
    void SetQuadDepth(float depth) { Vector3f pos = quad.Transform.GetPosition(); quad.Transform.SetPosition(Vector3f(pos.x, pos.y, depth)); }

    DrawingQuad(void);
    DrawingQuad(const DrawingQuad & cpy);

    void Render(const RenderInfo & info, Material & mat) { mat.Render(info, meshes); }

private:

    static const Vertex vertices[4];
    static const unsigned int indices[6];

    Mesh quad;
    std::vector<const Mesh*> meshes;

    static VertexIndexData vid;
};