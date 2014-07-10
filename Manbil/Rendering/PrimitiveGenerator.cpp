#include "PrimitiveGenerator.h"


void PrimitiveGenerator::GenerateCube(std::vector<VertexPosTex1Normal> & outVertices, std::vector<unsigned int> & outIndices, bool smoothNormals,
                                      bool faceInward, Vector2f xFaceScaling, Vector2f yFaceScaling, Vector2f zFaceScaling, Vector3f minCorner, Vector3f maxCorner)
{
    unsigned int index = outIndices.size();
    float faceNormalScale = (faceInward ? -1.0f : 1.0f);

    //X min face.
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(minCorner,
                                                              Vector2f(0.0f, 0.0f).ComponentProduct(xFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                                              Vector2f(0.0f, 1.0f).ComponentProduct(xFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                                              Vector2f(1.0f, 0.0f).ComponentProduct(xFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                                              Vector2f(1.0f, 1.0f).ComponentProduct(xFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(-1, 0, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    index += 4;

    //X max face.
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                                              Vector2f(0.0f, 0.0f).ComponentProduct(xFaceScaling),
                                                              (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                                              Vector2f(0.0f, 1.0f).ComponentProduct(xFaceScaling),
                                                              (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                                              Vector2f(1.0f, 0.0f).ComponentProduct(xFaceScaling),
                                                              (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(maxCorner,
                                                              Vector2f(1.0f, 1.0f).ComponentProduct(xFaceScaling),
                                                              (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(1, 0, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    index += 4;


    //Y min face.
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(minCorner,
                                                              Vector2f(0.0f, 0.0f).ComponentProduct(yFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                                              Vector2f(0.0f, 1.0f).ComponentProduct(yFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                                              Vector2f(1.0f, 0.0f).ComponentProduct(yFaceScaling),
                                                              (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                                              Vector2f(1.0f, 1.0f).ComponentProduct(yFaceScaling),
                                                              (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(0, -1, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    index += 4;

    //Y max face.
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                                              Vector2f(0.0f, 0.0f).ComponentProduct(yFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                                              Vector2f(0.0f, 1.0f).ComponentProduct(yFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                                              Vector2f(1.0f, 0.0f).ComponentProduct(yFaceScaling),
                                                              (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(maxCorner,
                                                              Vector2f(1.0f, 1.0f).ComponentProduct(yFaceScaling),
                                                              (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(0, 1, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    index += 4;


    //Z min face.
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(minCorner,
                                                              Vector2f(0.0f, 0.0f).ComponentProduct(zFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                                              Vector2f(0.0f, 1.0f).ComponentProduct(zFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                                              Vector2f(1.0f, 0.0f).ComponentProduct(zFaceScaling),
                                                              (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                                              Vector2f(1.0f, 1.0f).ComponentProduct(zFaceScaling),
                                                              (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(0, 0, -1)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    index += 4;


    //Z max face.
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                                              Vector2f(0.0f, 0.0f).ComponentProduct(zFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                                              Vector2f(0.0f, 1.0f).ComponentProduct(zFaceScaling),
                                                              (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                                              Vector2f(1.0f, 0.0f).ComponentProduct(zFaceScaling),
                                                              (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), VertexPosTex1Normal(Vector3f(maxCorner.x, maxCorner.y, maxCorner.z),
                                                              Vector2f(1.0f, 1.0f).ComponentProduct(zFaceScaling),
                                                              (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(0, 0, 1)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    index += 4;
}

void PrimitiveGenerator::GenerateCubemapCube(std::vector<CubemapVertex> & outVertices, std::vector<unsigned int> & outIndices, bool smoothNormals,
                                             bool faceInward, Vector3f minCorner, Vector3f maxCorner)
{
    unsigned int index = outIndices.size();
    float faceNormalScale = (faceInward ? -1.0f : 1.0f);

    //X min face.
    outVertices.insert(outVertices.end(), CubemapVertex(minCorner, (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z), (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z), (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z), (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(-1, 0, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    index += 4;

    //X max face.
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z), (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z), (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z), (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(maxCorner, (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(1, 0, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    index += 4;


    //Y min face.
    outVertices.insert(outVertices.end(), CubemapVertex(minCorner, (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z), (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z), (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z), (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(0, -1, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    index += 4;

    //Y max face.
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z), (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z), (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z), (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(maxCorner, (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(0, 1, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    index += 4;


    //Z min face.
    outVertices.insert(outVertices.end(), CubemapVertex(minCorner, (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z), (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z), (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z), (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(0, 0, -1)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    index += 4;


    //Z max face.
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z), (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z), (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z), (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.insert(outVertices.end(), CubemapVertex(Vector3f(maxCorner.x, maxCorner.y, maxCorner.z), (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(0, 0, 1)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 2);
        outIndices.insert(outIndices.end(), index + 3);
    }
    else
    {
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 1);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index);
        outIndices.insert(outIndices.end(), index + 3);
        outIndices.insert(outIndices.end(), index + 2);
    }
    index += 4;
}