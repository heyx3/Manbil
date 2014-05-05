#pragma once

#include <vector>
#include "../Vertex.h"


//Generates 3D primitive volumes.
class PrimitiveGenerator
{
public:

    //TODO: Allow flipping normals/index order.
    //TODO: Allow scaling the texture coordinate along each axis.
    static void GenerateCube(std::vector<Vertex> & outVertices, std::vector<unsigned int> & outIndices, bool smoothNormals,
                             Vector3f minCorner = Vector3f(-1.0f, -1.0f, -1.0f), Vector3f maxCorner = Vector3f(1.0f, 1.0f, 1.0f));
};