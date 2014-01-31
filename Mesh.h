#pragma once

#include "ShaderHandler.h"
#include "Math/Higher Math/TransformObject.h"


//Wraps the rendering of vertices using a material.
class Mesh
{
public:

	TransformObject Transform;

	Mesh(PrimitiveTypes pType, int numbVIData = 0, VertexIndexData * viDataArray = 0)
		: primType(pType), nVIData(numbVIData)
	{
		if (viDataArray == 0)
		{
			viData = 0;
			return;
		}


		viData = new VertexIndexData[numbVIData];
		for (int i = 0; i < numbVIData; ++i)
		{
			viData[i] = viDataArray[i];
		}
	}
	Mesh(const Mesh & cpy)
		: primType(cpy.primType), nVIData(cpy.nVIData)
	{
		if (cpy.viData == 0)
		{
			viData = 0;
			return;
		}

		viData = new VertexIndexData[nVIData];
		for (int i = 0; i < nVIData; ++i)
			viData[i] = cpy.viData[i];
	}
	~Mesh(void) { if (viData != 0) delete[] viData; }

	int GetNumbVertexIndexData(void) const { return nVIData; }

	const VertexIndexData & GetVertexIndexData(int datNumb) const { return viData[datNumb]; }
	void SetVertexIndexData(const VertexIndexData * toCopy, int numbToCopy) { if (viData != 0) delete[] viData; nVIData = numbToCopy; viData = new VertexIndexData[nVIData]; for (int i = 0; i < nVIData; ++i) viData[i] = VertexIndexData(toCopy[i]); }

	PrimitiveTypes GetPrimType(void) const { return primType; }

	void operator=(const Mesh & other) { Transform = other.Transform; primType = other.primType; SetVertexIndexData(other.viData, other.nVIData); }

private:

	PrimitiveTypes primType;

	VertexIndexData * viData;
	int nVIData;
};