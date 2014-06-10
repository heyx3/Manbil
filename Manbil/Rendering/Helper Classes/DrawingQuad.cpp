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



void DrawingQuad::InitializeQuadData(void)
{
    if (!IsInitialized())
    {
        RenderObjHandle vbo, ibo;
        RenderDataHandler::CreateVertexBuffer(vbo, vertices, 4, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);
        RenderDataHandler::CreateIndexBuffer(ibo, indices, 6, RenderDataHandler::BufferPurpose::UPDATE_ONCE_AND_DRAW);

        vid = VertexIndexData(4, vbo, 6, ibo);
    }
}
void DrawingQuad::DestroyQuadData(void)
{
    if (IsInitialized())
    {
        RenderDataHandler::DeleteBuffer(vid.GetVerticesHandle());
        RenderDataHandler::DeleteBuffer(vid.GetIndicesHandle());
        vid = VertexIndexData(-1, 0, -1, 0);
    }
}


DrawingQuad::DrawingQuad(void)
    : quad(PrimitiveTypes::TriangleList), origin(0.0f, 0.0f)
{
    assert(IsInitialized());

    //Set up this quad's mesh.
    quad.SetVertexIndexData(&vid, 1);
    meshes.insert(meshes.end(), &quad);
}
DrawingQuad::DrawingQuad(const DrawingQuad & cpy)
    : quad(PrimitiveTypes::TriangleList), origin(cpy.origin)
{
    quad = cpy.quad;
    meshes.insert(meshes.end(), &quad);
}

DrawingQuad & DrawingQuad::operator=(const DrawingQuad & cpy)
{
    quad.Transform = cpy.quad.Transform;
    origin = cpy.origin;

    return *this;
}

bool DrawingQuad::Render(RenderPasses pass, const RenderInfo & info, const UniformDictionary & params, Material & mat)
{
    assert(mat.GetAttributeData() == GetAttributeData());

    Vector3f scale = quad.Transform.GetScale();
    Vector2f scale2d(scale.x, scale.y);

    //Vector2f delta = origin.ComponentProduct(scale2d);
    Vector2f delta = origin;

    quad.Transform.IncrementPosition(Vector3f(delta.x, delta.y, 0.0f));
    bool rendered = mat.Render(pass, info, meshes, params);
    quad.Transform.IncrementPosition(-Vector3f(delta.x, delta.y, 0.0f));

    return rendered;
}