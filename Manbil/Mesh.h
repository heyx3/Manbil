#pragma once

#include "ShaderHandler.h"
#include "Math/Higher Math/TransformObject.h"
#include "Rendering/Materials/UniformCollections.h"

//TODO: Use the array of VertexIndexData as an LOD system -- each LOD is one VertexIndexData instance.

//Wraps the rendering of vertices using a material.
class Mesh
{
public:

    //TODO: Add matrix and int array values. Add matrix array uniform set functions to RenderDataHandler. Next, make sure that Material::Render checks all these collections and that all code in Mesh.cpp uses them. Finally, change the functions in Materials.h to either take a mesh or a material, not individual uniform lists.
    UniformDictionary Uniforms;
	TransformObject Transform;


	Mesh(PrimitiveTypes pType, int numbVIData = 0, VertexIndexData * viDataArray = 0);
	Mesh(const Mesh & cpy);
	~Mesh(void) { if (viData != 0) delete[] viData; }


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