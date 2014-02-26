#pragma once

#include "ShaderHandler.h"
#include "Math/Higher Math/TransformObject.h"
#include "Rendering/Materials/UniformCollections.h"

//TODO: Use the array of VertexIndexData as an LOD system -- each LOD is one VertexIndexData instance.

//Wraps the rendering of vertices using a material.
class Mesh
{
public:

    UniformDictionary Uniforms;
	TransformObject Transform;


	Mesh(PrimitiveTypes pType, int numbVIData = 0, VertexIndexData * viDataArray = 0);
	Mesh(const Mesh & cpy);
	~Mesh(void) { if (viData != 0) delete[] viData; }
    void DeleteVertexIndexBuffers(void)
    {
        for (unsigned int i = 0; i < nVIData; ++i)
        {
            RenderDataHandler::DeleteBuffer(viData[i].GetVerticesHandle());
            RenderDataHandler::DeleteBuffer(viData[i].GetIndicesHandle());
        }
    }
    

	int GetNumbVertexIndexData(void) const { return nVIData; }

	const VertexIndexData & GetVertexIndexData(int datNumb) const { return viData[datNumb]; }
    void SetVertexIndexData(const VertexIndexData & toCopy) { SetVertexIndexData(&toCopy, 1); }
	void SetVertexIndexData(const VertexIndexData * toCopy, int numbToCopy);

	PrimitiveTypes GetPrimType(void) const { return primType; }

	void operator=(const Mesh & other);

private:

	PrimitiveTypes primType;

	VertexIndexData * viData;
	int nVIData;
};