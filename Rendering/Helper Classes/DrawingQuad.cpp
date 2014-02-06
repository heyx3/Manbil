#include "DrawingQuad.h"

const Vertex DrawingQuad::vertices[4] =
{
    Vertex(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(0.0f, 0.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector3f(0.0f, 0.0f, 1.0f)),
    Vertex(Vector3f(1.0f, -1.0f, 0.0f), Vector2f(1.0f, 0.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector3f(0.0f, 0.0f, 1.0f)),
    Vertex(Vector3f(-1.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector3f(0.0f, 0.0f, 1.0f)),
    Vertex(Vector3f(1.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector3f(0.0f, 0.0f, 1.0f)),
};
const unsigned int DrawingQuad::indices[6] =
{
    0, 1, 3,
    0, 2, 3,
};

VertexIndexData DrawingQuad::vid = VertexIndexData(-1, 0, -1, 0);


DrawingQuad::DrawingQuad(void)
    : quad(PrimitiveTypes::Triangles)
{
    //Set up the vertices if they haven't been already.
    if (vid.GetVerticesCount() < 0)
    {
        RenderObjHandle vbo, ibo;
        RenderDataHandler::CreateVertexBuffer(vbo, vertices, 4, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
        RenderDataHandler::CreateIndexBuffer(ibo, indices, 6, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);

        vid = VertexIndexData(4, vbo, 6, ibo);
    }

    //Set up this quad's mesh.
    quad.SetVertexIndexData(&vid, 1);
    meshes.insert(meshes.end(), &quad);
}
DrawingQuad::DrawingQuad(const DrawingQuad & cpy)
    : quad(PrimitiveTypes::Triangles)
{
    quad = cpy.quad;
    meshes.insert(meshes.end(), &quad);
}