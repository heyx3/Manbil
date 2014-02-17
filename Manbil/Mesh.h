#pragma once

/*

#include "ShaderHandler.h"
#include "Math/Higher Math/TransformObject.h"
#include "Material.h"

//TODO: Use the array of VertexIndexData as an LOD system -- each LOD is one VertexIndexData instance.

//Wraps the rendering of vertices using a material.
class Mesh
{
public:

    template<typename DataType>
    //Represents a single uniform value with up to four components.
    struct UniformValue
    {
    public:
        DataType Data[4]; //Uniforms are a max of 4 components long.
        unsigned int NData;
        UniformValue(const DataType * data, unsigned int nData) : NData(nData) { for (int i = 0; i < nData; ++i) Data[i] = data[i]; }
        UniformValue(void) : NData(0) { }
        UniformValue(float f) : NData(1) { Data[0] = f; }
        UniformValue(Vector2f fs) : NData(2) { Data[0] = fs.x; Data[1] = fs.y; }
        UniformValue(Vector3f fs) : NData(3) { Data[0] = fs.x; Data[1] = fs.y; Data[2] = fs.z; }
        UniformValue(Vector4f fs) : NData(4) { Data[0] = fs.x; Data[1] = fs.y; Data[2] = fs.z; Data[3] = fs.w; }
        void SetData(DataType data[4]) { for (int i = 0; i < 3; ++i) Data[i] = data[i]; }
    };
    template<typename DataType>
    //Represents a single array uniform value with up to four components.
    struct UniformArrayValue
    {
    public:
        DataType * Data;
        unsigned int NArrayElements, NDataPerElement;
        UniformArrayValue(const DataType * data, unsigned int nArrayElements, unsigned int nDataPerElement)
            : NArrayElements(nArrayElements), NDataPerElement(nDataPerElement)
        {
            assert(nDataPerElement > 0 && nDataPerElement < 5);
            Data = new DataType[NArrayElements * NDataPerElement];
            for (int i = 0; i < NArrayElements * NDataPerElement; ++i)
                Data[i] = data[i];
        }
        UniformArrayValue(const UniformArrayValue<DataType> & cpy)
            : NArrayElements(cpy.NArrayElements), NDataPerElement(cpy.NDataPerElement)
        {
            if (cpy.Data != 0)
            {
                Data = new DataType[NArrayElements * NDataPerElement];
                for (int i = 0; i < NArrayElements * NDataPerElement; ++i)
                    Data[i] = cpy.Data[i];
            }
        }
        UniformArrayValue(void) : Data(0), NArrayElements(0), NDataPerElement(0) { }
        ~UniformArrayValue(void)
        {
            delete[] Data;
        }
        void SetData(const DataType * data)
        {
            assert(Data != 0);
            for (int i = 0; i < NArrayElements * NDataPerElement; ++i)
                Data[i] = data[i];
        }
        void SetData(const DataType * data, unsigned int nArrayElements, unsigned int nDataPerElement)
        {
            NArrayElements = nArrayElements;
            NDataPerElement = nDataPerElement;
            Data = new DataType[NArrayElements * NDataPerElement];
            for (int i = 0; i < NArrayElements * NDataPerElement; ++i)
                Data[i] = data[i];
        }
    };

    std::vector<PassSamplers> TextureSamplers;
    //TODO: Add matrix array values. Add matrix array uniform set functions to RenderDataHandler. Next, make sure that Material::Render checks all these collections and that all code in Mesh.cpp uses them. Finally, change the functions in Materials.h to either take a mesh or a material, not individual uniform lists.
    std::unordered_map<std::string, UniformValue<float>> FloatUniformValues;
    std::unordered_map<std::string, UniformArrayValue<float>> FloatArrayUniformValues;
    std::unordered_map<std::string, UniformValue<int>> IntUniformValues;
    std::unordered_map<std::string, UniformArrayValue<int>> IntArrayUniformValues;
    std::unordered_map<std::string, Matrix4f> MatUniformValues;


	TransformObject Transform;


	Mesh(PrimitiveTypes pType, int numbVIData = 0, VertexIndexData * viDataArray = 0);
	Mesh(const Mesh & cpy);
	~Mesh(void) { if (viData != 0) delete[] viData; }

	int GetNumbVertexIndexData(void) const { return nVIData; }

	const VertexIndexData & GetVertexIndexData(int datNumb) const { return viData[datNumb]; }
    void SetVertexIndexData(VertexIndexData toCopy) { SetVertexIndexData(&toCopy, 1); }
	void SetVertexIndexData(const VertexIndexData * toCopy, int numbToCopy);

	PrimitiveTypes GetPrimType(void) const { return primType; }

	void operator=(const Mesh & other);

private:

	PrimitiveTypes primType;

	VertexIndexData * viData;
	int nVIData;
};

*/