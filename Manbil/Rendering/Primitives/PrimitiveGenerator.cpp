#include "PrimitiveGenerator.h"


RenderIOAttributes PrimitiveGenerator::CubemapVertex::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderIOAttributes::Attribute(3, false, "vIn_Pos"),
                              RenderIOAttributes::Attribute(3, true, "vIn_Normal"));
}

void PrimitiveGenerator::GenerateCube(std::vector<VertexPosUVNormal>& outVertices,
                                      std::vector<unsigned int>& outIndices,
                                      bool smoothNormals, bool faceInward,
                                      Vector2f xFaceScaling, Vector2f yFaceScaling, Vector2f zFaceScaling,
                                      Vector3f minCorner, Vector3f maxCorner)
{
    unsigned int index = outIndices.size();
    float faceNormalScale = (faceInward ? -1.0f : 1.0f);

    //X min face.
    outVertices.push_back(VertexPosUVNormal(minCorner,
                                            Vector2f(0.0f, 0.0f).ComponentProduct(xFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, -1, -1).Normalized() :
                                                Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                            Vector2f(0.0f, 1.0f).ComponentProduct(xFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, 1, -1).Normalized() :
                                                Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                            Vector2f(1.0f, 0.0f).ComponentProduct(xFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, -1, 1).Normalized() :
                                                Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                            Vector2f(1.0f, 1.0f).ComponentProduct(xFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, 1, 1).Normalized() :
                                                Vector3f(-1, 0, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    index += 4;

    //X max face.
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                            Vector2f(0.0f, 0.0f).ComponentProduct(xFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, -1, -1).Normalized() :
                                                Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                            Vector2f(0.0f, 1.0f).ComponentProduct(xFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, 1, -1).Normalized() :
                                                Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                            Vector2f(1.0f, 0.0f).ComponentProduct(xFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, -1, 1).Normalized() :
                                                Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(maxCorner,
                                            Vector2f(1.0f, 1.0f).ComponentProduct(xFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, 1, 1).Normalized() :
                                                Vector3f(1, 0, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    index += 4;


    //Y min face.
    outVertices.push_back(VertexPosUVNormal(minCorner,
                                            Vector2f(0.0f, 0.0f).ComponentProduct(yFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, -1, -1).Normalized() :
                                                Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                            Vector2f(0.0f, 1.0f).ComponentProduct(yFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, -1, 1).Normalized() :
                                                Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                            Vector2f(1.0f, 0.0f).ComponentProduct(yFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, -1, -1).Normalized() :
                                                Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                            Vector2f(1.0f, 1.0f).ComponentProduct(yFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, -1, 1).Normalized() :
                                                Vector3f(0, -1, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    index += 4;

    //Y max face.
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                            Vector2f(0.0f, 0.0f).ComponentProduct(yFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, 1, -1).Normalized() :
                                                Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                            Vector2f(0.0f, 1.0f).ComponentProduct(yFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, 1, 1).Normalized() :
                                                Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                            Vector2f(1.0f, 0.0f).ComponentProduct(yFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, 1, -1).Normalized() :
                                                Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(maxCorner,
                                            Vector2f(1.0f, 1.0f).ComponentProduct(yFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, 1, 1).Normalized() :
                                                Vector3f(0, 1, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    index += 4;


    //Z min face.
    outVertices.push_back(VertexPosUVNormal(minCorner,
                                            Vector2f(0.0f, 0.0f).ComponentProduct(zFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, -1, -1).Normalized() :
                                                Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                            Vector2f(0.0f, 1.0f).ComponentProduct(zFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, 1, -1).Normalized() :
                                                Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                            Vector2f(1.0f, 0.0f).ComponentProduct(zFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, -1, -1).Normalized() :
                                                Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                            Vector2f(1.0f, 1.0f).ComponentProduct(zFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, 1, -1).Normalized() :
                                                Vector3f(0, 0, -1)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    index += 4;


    //Z max face.
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                            Vector2f(0.0f, 0.0f).ComponentProduct(zFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, -1, 1).Normalized() :
                                                Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                            Vector2f(0.0f, 1.0f).ComponentProduct(zFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(-1, 1, 1).Normalized() :
                                                Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                            Vector2f(1.0f, 0.0f).ComponentProduct(zFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, -1, 1).Normalized() :
                                                Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.push_back(VertexPosUVNormal(Vector3f(maxCorner.x, maxCorner.y, maxCorner.z),
                                            Vector2f(1.0f, 1.0f).ComponentProduct(zFaceScaling),
                                            (smoothNormals ?
                                                Vector3f(1, 1, 1).Normalized() :
                                                Vector3f(0, 0, 1)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    index += 4;
}

void PrimitiveGenerator::GenerateCubemapCube(std::vector<CubemapVertex>& outVertices,
                                             std::vector<unsigned int>& outIndices,
                                             bool smoothNormals, bool faceInward,
                                             Vector3f minCorner, Vector3f maxCorner)
{
    unsigned int index = outIndices.size();
    float faceNormalScale = (faceInward ? -1.0f : 1.0f);

    //X min face.
    outVertices.push_back(CubemapVertex(minCorner,
                                        (smoothNormals ?
                                            Vector3f(-1, -1, -1).Normalized() :
                                            Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, 1, -1).Normalized() :
                                            Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, -1, 1).Normalized() :
                                            Vector3f(-1, 0, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, 1, 1).Normalized() :
                                            Vector3f(-1, 0, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    index += 4;

    //X max face.
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, -1, -1).Normalized() :
                                            Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, 1, -1).Normalized() :
                                            Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, -1, 1).Normalized() :
                                            Vector3f(1, 0, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(maxCorner,
                                        (smoothNormals ?
                                            Vector3f(1, 1, 1).Normalized() :
                                            Vector3f(1, 0, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    index += 4;


    //Y min face.
    outVertices.push_back(CubemapVertex(minCorner,
                                        (smoothNormals ?
                                            Vector3f(-1, -1, -1).Normalized() :
                                            Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, -1, 1).Normalized() :
                                            Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, -1, -1).Normalized() :
                                            Vector3f(0, -1, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, -1, 1).Normalized() :
                                            Vector3f(0, -1, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    index += 4;

    //Y max face.
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, 1, -1).Normalized() :
                                            Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, 1, 1).Normalized() :
                                            Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, 1, -1).Normalized() :
                                            Vector3f(0, 1, 0)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(maxCorner,
                                        (smoothNormals ?
                                            Vector3f(1, 1, 1).Normalized() :
                                            Vector3f(0, 1, 0)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    index += 4;


    //Z min face.
    outVertices.push_back(CubemapVertex(minCorner,
                                        (smoothNormals ?
                                            Vector3f(-1, -1, -1).Normalized() :
                                            Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, 1, -1).Normalized() :
                                            Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, -1, -1).Normalized() :
                                            Vector3f(0, 0, -1)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, 1, -1).Normalized() :
                                            Vector3f(0, 0, -1)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    index += 4;


    //Z max face.
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, -1, 1).Normalized() :
                                            Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(-1, 1, 1).Normalized() :
                                            Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, -1, 1).Normalized() :
                                            Vector3f(0, 0, 1)) * faceNormalScale));
    outVertices.push_back(CubemapVertex(Vector3f(maxCorner.x, maxCorner.y, maxCorner.z),
                                        (smoothNormals ?
                                            Vector3f(1, 1, 1).Normalized() :
                                            Vector3f(0, 0, 1)) * faceNormalScale));
    if (faceInward)
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 1);
        outIndices.push_back(index);
        outIndices.push_back(index + 2);
        outIndices.push_back(index + 3);
    }
    else
    {
        outIndices.push_back(index);
        outIndices.push_back(index + 1);
        outIndices.push_back(index + 3);
        outIndices.push_back(index);
        outIndices.push_back(index + 3);
        outIndices.push_back(index + 2);
    }
    index += 4;
}