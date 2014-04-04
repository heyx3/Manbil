#include "Mesh.h"


Mesh::Mesh(PrimitiveTypes pType, int numbVIData, VertexIndexData * viDataArray)
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
Mesh::Mesh(const Mesh & cpy)
    : primType(cpy.primType), nVIData(cpy.nVIData)
{
    Uniforms = cpy.Uniforms;

    Transform = cpy.Transform;

    if (cpy.viData == 0)
    {
        viData = 0;
        return;
    }

    viData = new VertexIndexData[nVIData];
    for (unsigned int i = 0; i < nVIData; ++i)
        viData[i] = cpy.viData[i];
}

void Mesh::SetVertexIndexData(const VertexIndexData * toCopy, int numbToCopy)
{
    if (viData != 0) delete[] viData;
    nVIData = numbToCopy;
    viData = new VertexIndexData[nVIData];
    for (unsigned int i = 0; i < nVIData; ++i)
        viData[i] = VertexIndexData(toCopy[i]);
}

void Mesh::operator=(const Mesh & other)
{
    Transform = other.Transform;
    primType = other.primType;
    SetVertexIndexData(other.viData, other.nVIData);

    Uniforms = other.Uniforms;
}