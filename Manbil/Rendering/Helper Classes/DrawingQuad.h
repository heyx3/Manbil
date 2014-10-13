#pragma once

#include "../../OpenGLIncludes.h"
#include "../../Mesh.h"
#include "../../Material.h"

//Represents a simple 1x1 square.
//Uses the VertexPosTex1Normal struct for the vertices.
class DrawingQuad
{
public:
 
    //Must be called before creating any quads.
    static void InitializeQuadData(void);
    //Must be called after all quads are deleted and no more will be created.
    //If more quads need to be created after calling this, just restart the system by calling "InitializeQuadData()" again.
    static void DestroyQuadData(void);

    //Gets whether "InitializeQuadData" has been called yet.
    static bool IsInitialized(void) { return vid.GetVerticesCount() >= 0; }

    //Gets the vertex attributes that this quad uses.
    static ShaderInOutAttributes GetAttributeData(void) { return VertexPosTex1Normal::GetAttributeData(); }


    Mesh & GetMesh(void) { return quad; }
    const Mesh & GetMesh(void) const { return quad; }

    Vector2f GetPos(void) const { return Vector2f(quad.Transform.GetPosition().x, quad.Transform.GetPosition().y); }
    float GetDepth(void) const { return quad.Transform.GetPosition().z; }
    Vector2f GetSize(void) const { return Vector2f(quad.Transform.GetScale().x, quad.Transform.GetScale().y); }
    float GetRotation(void) const { return quad.Transform.GetRotationAngles().z; }
    Vector2f GetOrigin(void) const { return origin; }

    void SetBounds(Vector2f min, Vector2f max) { SetPos((min + max) * 0.5f); SetSize((max - min) * 0.5f); }
    void SetPos(Vector2f pos) { quad.Transform.SetPosition(Vector3f(pos.x, pos.y, GetDepth())); }
    void IncrementPos(Vector2f increment) { quad.Transform.IncrementPosition(Vector3f(increment.x, increment.y, 0.0f)); }
    void SetSize(Vector2f size) { quad.Transform.SetScale(Vector3f(size.x, size.y, 1.0f)); }
    void MakeSizePositive(void) { quad.Transform.SetScale(Vector3f(BasicMath::Abs(quad.Transform.GetScale().x), BasicMath::Abs(quad.Transform.GetScale().y), 1.0f)); }
    void SetDepth(float depth) { Vector3f pos = quad.Transform.GetPosition(); quad.Transform.SetPosition(Vector3f(pos.x, pos.y, depth)); }
    void SetRotation(float newRot) { quad.Transform.SetRotation(Vector3f(0.0f, 0.0f, newRot)); }
    void Rotate(float amount) { quad.Transform.Rotate(Vector3f(0.0f, 0.0f, amount)); }
    void SetOrigin(Vector2f newOrg) { origin = newOrg; }

    DrawingQuad(void);
    DrawingQuad(const DrawingQuad & cpy);

    DrawingQuad & operator=(const DrawingQuad & cpy);

    bool Render(const RenderInfo & info, const UniformDictionary & params, Material & mat);

private:

    Mesh quad;
    std::vector<const Mesh*> meshes;

    Vector2f origin;

    static const VertexPosTex1Normal vertices[4];
    static const unsigned int indices[6];
    static VertexIndexData vid;
};