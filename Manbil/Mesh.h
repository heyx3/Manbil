#pragma once

#include "ShaderHandler.h"
#include "Math/Higher Math/TransformObject.h"
#include "Rendering/Materials/UniformCollections.h"
#include "RenderDataHandler.h"
#include "Vertices.h"


//TODO: Use VID array to split up vertex buffers into multiple draw calls to get an optimal amount of triangles per draw call.
//TODO: Come up with an LOD system.

//Wraps the rendering of vertices using a material.
class Mesh
{
public:

    UniformDictionary Uniforms;
	TransformObject Transform;
    VertexAttributes VertAttributes;
    

	Mesh(PrimitiveTypes pType, VertexAttributes attributes, int numbVIData = 0, VertexIndexData * viDataArray = 0);
	Mesh(const Mesh & cpy);

	~Mesh(void) { if (viData != 0) delete[] viData; }

    //Deletes the vertex/index buffers held by this mesh from OpenGL.
    void DestroyVertexIndexBuffers(void)
    {
        for (unsigned int i = 0; i < nVIData; ++i)
        {
            RenderDataHandler::DeleteBuffer(viData[i].GetVerticesHandle());
            RenderDataHandler::DeleteBuffer(viData[i].GetIndicesHandle());
            viData[i] = VertexIndexData();
        }
    }
    

	unsigned int GetNumbVertexIndexData(void) const { return nVIData; }

	const VertexIndexData & GetVertexIndexData(unsigned int datNumb) const { return viData[datNumb]; }
    void SetVertexIndexData(const VertexIndexData & toCopy) { SetVertexIndexData(&toCopy, 1); }
	void SetVertexIndexData(const VertexIndexData * toCopy, int numbToCopy);

	PrimitiveTypes GetPrimType(void) const { return primType; }

	void operator=(const Mesh & other);

private:

	PrimitiveTypes primType;

	VertexIndexData * viData;
	unsigned int nVIData;
};