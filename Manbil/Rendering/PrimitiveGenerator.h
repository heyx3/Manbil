#pragma once

#include <vector>
#include "../Vertices.h"


//TODO: Move into the folder that matches this file's filter.

//Generates 3D primitive volumes.
class PrimitiveGenerator
{
public:

    static void GenerateCube(std::vector<VertexPosTex1Normal> & outVertices, std::vector<unsigned int> & outIndices, bool smoothNormals,
                             bool faceInward, Vector2f xFaceScaling = Vector2f(1.0f, 1.0f), Vector2f yFaceScaling = Vector2f(1.0f, 1.0f), Vector2f zFaceScaling = Vector2f(1.0f, 1.0f),
                             Vector3f minCorner = Vector3f(-1.0f, -1.0f, -1.0f), Vector3f maxCorner = Vector3f(1.0f, 1.0f, 1.0f));


    //Vertex used for generated cubemap. Contains a position and a normal.
    struct CubemapVertex
    {
    public:
        Vector3f Pos;
        Vector3f Normal;
        CubemapVertex(Vector3f pos = Vector3f(), Vector3f normal = Vector3f(1.0f, 0.0f, 0.0f)) : Pos(pos), Normal(normal) { }
        static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, 3, false, true, "in_pos", "in_normal"); }
    };
    static void GenerateCubemapCube(std::vector<CubemapVertex> & outVertices, std::vector<unsigned int> & outIndices, bool smoothNormals,
                                    bool faceInward, Vector3f minCorner = Vector3f(-1.0f, -1.0f, -1.0f), Vector3f maxCorner = Vector3f(1.0f, 1.0f, 1.0f));
};