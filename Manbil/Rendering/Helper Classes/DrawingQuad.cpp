#include "DrawingQuad.h"

const VertexPosTex1Normal DrawingQuad::vertices[4] =
{
    VertexPosTex1Normal(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f)),
    VertexPosTex1Normal(Vector3f(1.0f, -1.0f, 0.0f), Vector2f(1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f)),
    VertexPosTex1Normal(Vector3f(-1.0f, 1.0f, 0.0f), Vector2f(0.0f, 1.0f), Vector3f(0.0f, 0.0f, 1.0f)),
    VertexPosTex1Normal(Vector3f(1.0f, 1.0f, 0.0f), Vector2f(1.0f, 1.0f), Vector3f(0.0f, 0.0f, 1.0f)),
};
const unsigned int DrawingQuad::indices[6] =
{
    0, 1, 3,
    0, 3, 2,
};

VertexIndexData DrawingQuad::vid = VertexIndexData(-1, 0, -1, 0);


DrawingQuad::DrawingQuad(void)
    : quad(PrimitiveTypes::Triangles, GetAttributeData()), origin(0.0f, 0.0f)
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
    : quad(PrimitiveTypes::Triangles, GetAttributeData()), origin(cpy.origin)
{
    quad = cpy.quad;
    meshes.insert(meshes.end(), &quad);
}

DrawingQuad & DrawingQuad::operator=(const DrawingQuad & cpy)
{
    quad.Transform = cpy.quad.Transform;
    quad.Uniforms = cpy.quad.Uniforms;
    origin = cpy.origin;

    return *this;
}

bool DrawingQuad::Render(RenderPasses pass, const RenderInfo & info, Material & mat)
{
    Vector3f scale = quad.Transform.GetScale();
    Vector2f scale2d(scale.x, scale.y);

    Vector3f delta = Vector3f(scale.x * 0.5f, scale.y * 0.5f, 0.0f);
    delta -= Vector3f(origin.ComponentProduct(scale2d), 0.0f);

    return mat.Render(pass, info, meshes);
}