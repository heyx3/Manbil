#include "DrawingQuad.h"


DrawingQuad* DrawingQuad::instance = 0;



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
		instance->mesh.SetVertexData(vertices, 4, Mesh::BUF_STATIC,
									 VertexPosUV::GetVertexAttributes());
		instance->mesh.SetIndexData(indices, 6, Mesh::BUF_STATIC);

    }
}
void DrawingQuad::DestroyQuad(void)
{
    assert(instance != 0);
    delete instance;
    instance = 0;
}

void DrawingQuad::Render(const Transform& transform, const RenderInfo& info,
						 const Material& material, const UniformDictionary& params)
{
	material.Render(mesh, transform, info, params);
}