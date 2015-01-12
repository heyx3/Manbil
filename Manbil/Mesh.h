#pragma once

#include "ShaderHandler.h"
#include "Math/Higher Math/TransformObject.h"
#include "RenderDataHandler.h"
#include "Vertices.h"


//TODO: Use VID array to split up vertex buffers into multiple draw calls to get an optimal amount of triangles per draw call?

//A set of vertices (and possibly indices) with a position, rotation, and scale.
//The vertex data is split into sub-meshes, stored in an std::vector.
class Mesh
{
public:
    
    //The mesh data belonging to this mesh.
    std::vector<VertexIndexData> SubMeshes;
    //Which mesh data to render.
    unsigned int CurrentSubMesh = 0;

	TransformObject Transform;
    PrimitiveTypes PrimType;
    

	Mesh(PrimitiveTypes pType = PrimitiveTypes::TriangleList,
         std::vector<VertexIndexData> subMeshData = std::vector<VertexIndexData>())
        : PrimType(pType), SubMeshes(subMeshData) { }

    ~Mesh(void) { DestroySubMeshes(); }


    //Must be called before destroying this Mesh if this Mesh has any sub-meshes.
    void DestroySubMeshes(void);
    
    //Adds the given vertex data to this mesh's sub-mesh collection at the given index.
    //If the given index is less than 0, the sub-mesh will be inserted at the end of the collection.
    void AddSubMesh(VertexIndexData subMesh, int location = -1);
};