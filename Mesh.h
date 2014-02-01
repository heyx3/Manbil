#pragma once

#include "ShaderHandler.h"
#include "Math/Higher Math/TransformObject.h"
#include "Material.h"


//Wraps the rendering of vertices using a material.
class Mesh
{
public:

    template<typename DataType>
    //Represents a single uniform value with an arbitrary number of components.
    struct UniformValue
    {
    public:
        DataType * Data;
        unsigned int NData;
        UniformValue(DataType * data, unsigned int nData) : Data(data), NData(nData) { }
    };

    BufferObjHandle TextureSamplers[Material::TWODSAMPLERS];
    std::unordered_map<std::string, UniformValue<float>> FloatUniformValues;
    std::unordered_map<std::string, UniformValue<int>> IntUniformValues;
    std::unordered_map<std::string, Matrix4f> MatUniformValues;


	TransformObject Transform;


	Mesh(PrimitiveTypes pType, int numbVIData = 0, VertexIndexData * viDataArray = 0);
	Mesh(const Mesh & cpy);
	~Mesh(void) { if (viData != 0) delete[] viData; }

	int GetNumbVertexIndexData(void) const { return nVIData; }

	const VertexIndexData & GetVertexIndexData(int datNumb) const { return viData[datNumb]; }
	void SetVertexIndexData(const VertexIndexData * toCopy, int numbToCopy);

	PrimitiveTypes GetPrimType(void) const { return primType; }

	void operator=(const Mesh & other);

private:

	PrimitiveTypes primType;

	VertexIndexData * viData;
	int nVIData;
};