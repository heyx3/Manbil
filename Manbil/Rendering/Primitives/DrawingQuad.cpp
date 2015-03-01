#include "DrawingQuad.h"



DrawingQuad* DrawingQuad::instance = 0;

void DrawingQuad::SetBounds(Vector2f min, Vector2f max)
{
    SetPos((min + max) * 0.5f);
    SetSize((max - min) * 0.5f);
}
void DrawingQuad::SetPos(Vector2f pos)
{
    mesh.Transform.SetPosition(Vector3f(pos.x, pos.y, GetDepth()));
}
void DrawingQuad::IncrementPos(Vector2f increment)
{
    mesh.Transform.IncrementPosition(Vector3f(increment.x, increment.y, 0.0f));
}
void DrawingQuad::SetSize(Vector2f size)
{
    mesh.Transform.SetScale(Vector3f(size.x, size.y, 1.0f));
}
void DrawingQuad::MakeSizePositive(void)
{
    mesh.Transform.SetScale(Vector3f(Mathf::Abs(mesh.Transform.GetScale().x),
                                     Mathf::Abs(mesh.Transform.GetScale().y),
                                     1.0f));
}
void DrawingQuad::SetDepth(float depth)
{
    Vector3f pos = mesh.Transform.GetPosition();
    mesh.Transform.SetPosition(Vector3f(pos.x, pos.y, depth));
}
void DrawingQuad::SetRotation(float newRot)
{
    mesh.Transform.SetRotation(Vector3f(0.0f, 0.0f, newRot));
}
void DrawingQuad::Rotate(float amount)
{
    mesh.Transform.RotateRelative(Vector3f(0.0f, 0.0f, amount));
}

void DrawingQuad::InitializeQuad(void)
{
    if (instance == 0)
    {
        VertexPosUV vertices[4] =
        {
            VertexPosUV(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(0.0f, 0.0f)),
            VertexPosUV(Vector3f(1.0f, -1.0f, 0.0f), Vector2f(1.0f, 0.0f)),
            VertexPosUV(Vector3f(-1.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f)),
            VertexPosUV(Vector3f(1.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f)),
        };
        const unsigned int indices[6] =
        {
            0, 1, 3,
            0, 3, 2,
        };
        
        instance = new DrawingQuad();
        instance->mesh.SubMeshes.push_back(MeshData(false, PT_TRIANGLE_LIST));
        MeshData& dat = instance->mesh.SubMeshes[0];
        dat.SetVertexData(vertices, 4, MeshData::BUF_STATIC, VertexPosUV::GetAttributeData());
        dat.SetIndexData(indices, 6, MeshData::BUF_STATIC);
    }
}
void DrawingQuad::DestroyQuad(void)
{
    assert(instance != 0);
    delete instance;
}

void DrawingQuad::Render(const RenderInfo& info, const UniformDictionary& params, Material& mat)
{
    assert(mat.GetExpectedVertexData() == VertexPosUV::GetAttributeData());

    Vector3f scale = mesh.Transform.GetScale();
    Vector2f scale2d(scale.x, scale.y);

    mat.Render(info, &mesh, params);
}