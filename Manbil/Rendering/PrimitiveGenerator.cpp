#include "PrimitiveGenerator.h"


void PrimitiveGenerator::GenerateCube(std::vector<Vertex> & outVertices, std::vector<unsigned int> & outIndices, bool smoothNormals,
                                      Vector3f minCorner, Vector3f maxCorner)
{
    unsigned int index = outIndices.size();

    //X min face.
    outVertices.insert(outVertices.end(), Vertex(minCorner,
                                                 Vector2f(0.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(-1, 0, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                                 Vector2f(0.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(-1, 0, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                                 Vector2f(1.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(-1, 0, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                                 Vector2f(1.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(-1, 0, 0))));
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 1);
    outIndices.insert(outIndices.end(), index + 3);
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 3);
    outIndices.insert(outIndices.end(), index + 2);
    index += 4;

    //X max face.
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                                 Vector2f(0.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(1, 0, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                                 Vector2f(0.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(1, 0, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                                 Vector2f(1.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(1, 0, 0))));
    outVertices.insert(outVertices.end(), Vertex(maxCorner,
                                                 Vector2f(1.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(1, 0, 0))));
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 3);
    outIndices.insert(outIndices.end(), index + 1);
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 2);
    outIndices.insert(outIndices.end(), index + 3);
    index += 4;


    //Y min face.
    outVertices.insert(outVertices.end(), Vertex(minCorner,
                                                 Vector2f(0.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(0, -1, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                                 Vector2f(0.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(0, -1, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                                 Vector2f(1.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(0, -1, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                                 Vector2f(1.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(0, -1, 0))));
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 3);
    outIndices.insert(outIndices.end(), index + 1);
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 2);
    outIndices.insert(outIndices.end(), index + 3);
    index += 4;

    //Y max face.
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                                 Vector2f(0.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(0, 1, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                                 Vector2f(0.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(0, 1, 0))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                                 Vector2f(1.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(0, 1, 0))));
    outVertices.insert(outVertices.end(), Vertex(maxCorner,
                                                 Vector2f(1.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(0, 1, 0))));
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 3);
    outIndices.insert(outIndices.end(), index + 1);
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 2);
    outIndices.insert(outIndices.end(), index + 3);
    index += 4;


    //Z min face.
    outVertices.insert(outVertices.end(), Vertex(minCorner,
                                                 Vector2f(0.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(-1, -1, -1).Normalized() : Vector3f(0, 0, -1))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, maxCorner.y, minCorner.z),
                                                 Vector2f(0.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(-1, 1, -1).Normalized() : Vector3f(0, 0, -1))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, minCorner.y, minCorner.z),
                                                 Vector2f(1.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(1, -1, -1).Normalized() : Vector3f(0, 0, -1))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, maxCorner.y, minCorner.z),
                                                 Vector2f(1.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(1, 1, -1).Normalized() : Vector3f(0, 0, -1))));
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 3);
    outIndices.insert(outIndices.end(), index + 1);
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 2);
    outIndices.insert(outIndices.end(), index + 3);
    index += 4;


    //Z max face.
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, minCorner.y, maxCorner.z),
                                                 Vector2f(0.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(-1, -1, 1).Normalized() : Vector3f(0, 0, 1))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(minCorner.x, maxCorner.y, maxCorner.z),
                                                 Vector2f(0.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(-1, 1, 1).Normalized() : Vector3f(0, 0, 1))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, minCorner.y, maxCorner.z),
                                                 Vector2f(1.0f, 0.0f),
                                                 (smoothNormals ? Vector3f(1, -1, 1).Normalized() : Vector3f(0, 0, 1))));
    outVertices.insert(outVertices.end(), Vertex(Vector3f(maxCorner.x, maxCorner.y, maxCorner.z),
                                                 Vector2f(1.0f, 1.0f),
                                                 (smoothNormals ? Vector3f(1, 1, 1).Normalized() : Vector3f(0, 0, 1))));
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 3);
    outIndices.insert(outIndices.end(), index + 1);
    outIndices.insert(outIndices.end(), index);
    outIndices.insert(outIndices.end(), index + 2);
    outIndices.insert(outIndices.end(), index + 3);
    index += 4;
}