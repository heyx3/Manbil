#pragma once

#include "../../OpenGLIncludes.h"
#include "../../Mesh.h"
#include "../../Material.h"

//Represents a simple 1x1 square.
//Uses the VertexPosTex1Normal struct for the vertices.
class DrawingQuad
{
public:
 
    static VertexAttributes GetAttributeData(void) { return VertexPosTex1Normal::GetAttributeData(); }


    Mesh & GetMesh(void) { return quad; }
    const Mesh & GetMesh(void) const { return quad; }

    void SetPos(Vector2f pos) { quad.Transform.SetPosition(Vector3f(pos.x, pos.y, 0.0f)); }
    void SetSize(Vector2f size) { quad.Transform.SetScale(Vector3f(size.x, size.y, 1.0f)); }
    void SetDepth(float depth) { Vector3f pos = quad.Transform.GetPosition(); quad.Transform.SetPosition(Vector3f(pos.x, pos.y, depth)); }
    void SetRotation(float newRot) { quad.Transform.SetRotation(Vector3f(0.0f, 0.0f, newRot)); }
    void SetOrigin(Vector2f newOrg) { origin = newOrg; }
    void Rotate(float amount) { quad.Transform.Rotate(Vector3f(0.0f, 0.0f, amount)); }

    DrawingQuad(void);
    DrawingQuad(const DrawingQuad & cpy);

    DrawingQuad & operator=(const DrawingQuad & cpy);

    bool Render(RenderPasses pass, const RenderInfo & info, Material & mat);

private:

    Mesh quad;
    std::vector<const Mesh*> meshes;

    Vector2f origin;


    static const VertexPosTex1Normal vertices[4];
    static const unsigned int indices[6];

    static VertexIndexData vid;
};