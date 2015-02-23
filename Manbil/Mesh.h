#pragma once

#include "Rendering/Basic Rendering/MeshData.h"
#include "Math/Higher Math/TransformObject.h"


//A set of vertices (and possibly indices) with a position, rotation, and scale.
//The vertex data is split into sub-meshes, stored in an std::vector.
class Mesh
{
public:
    
    //The mesh data belonging to this mesh.
    std::vector<MeshData> SubMeshes;
    //Which mesh data to render.
    unsigned int CurrentSubMesh;

	TransformObject Transform;


    Mesh(void) : CurrentSubMesh(0) { }


private:

    Mesh(const Mesh& cpy) = delete;
    Mesh& operator=(const Mesh& cpy) = delete;
};