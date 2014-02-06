#include "DrawingQuad.h"

const Vertex DrawingQuad::vertices[4] =
{
    Vertex(Vector3f(0.0f, 0.0f, 0.0f), Vector2f(0.0f, 0.0f)),
    Vertex(Vector3f(1.0f, 0.0f, 0.0f), Vector2f(1.0f, 0.0f)),
    Vertex(Vector3f(0.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f)),
    Vertex(Vector3f(1.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f)),
};
const unsigned int DrawingQuad::indices[6] =
{
    1, 0, 3,
    0, 2, 3,
};

Mesh DrawingQuad::quad = Mesh(PrimitiveTypes::Triangles);

std::vector<const Mesh*> DrawingQuad::meshes = std::vector<const Mesh*>();



DrawingQuad::DrawingQuad(void)
{
    //Initialize the mesh if it hasn't been already.
    if (meshes.empty())
    {
        RenderObjHandle vbo, ibo;
        RenderDataHandler::CreateVertexBuffer(vbo, vertices, 4, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
        RenderDataHandler::CreateIndexBuffer(ibo, indices, 6, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
        
        VertexIndexData vid(4, vbo, 6, ibo);
        quad.SetVertexIndexData(&vid, 1);
        
        meshes.insert(meshes.end(), &quad);
    }
}