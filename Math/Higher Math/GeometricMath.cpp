#include "GeometricMath.h"

void GeometricMath::CalculateNormals(const Vector3f * vertices, const int * indices, int nVertices, int nIndices, Vector3f * outNormals)
{
	int * nNormals = new int[nVertices];
	int i;

	//Clear out the "out" array.
	for (i = 0; i < nVertices; ++i)
	{
		outNormals[i] = Vector3f();
		nNormals[i] = 0;
	}

	//Add in normals.
	Vector3f v1, v2, crossed;
	int ind1, ind2, ind3;
	for (i = 0; i < nIndices; i += 3)
	{
		ind1 = indices[i];
		ind2 = indices[i + 1];
		ind3 = indices[i + 2];

		//Get two vectors to cross.
		v1 = (vertices[ind1] - vertices[ind2]).Normalized();
        v2 = (vertices[ind1] - vertices[ind3]).Normalized();

		//Cross them.
        crossed = v1.Cross(v2).Normalized();
		
		//Add the crossed value into the total for the triangle's vertices.
		outNormals[ind1] += crossed;
		nNormals[ind1] += 1;
		outNormals[ind2] += crossed;
		nNormals[ind2] += 1;
		outNormals[ind3] += crossed;
		nNormals[ind3] += 1;
	}

	//Go through and get the final averaged normal.
	for (i = 0; i < nVertices; ++i)
	{
		outNormals[i] /= (float)nNormals[i];
		outNormals[i].FastNormalize();
	}

	delete [] nNormals;
}

void GeometricMath::CalculateNormals(const Vector3f * vertices, int nVertices, Vector3f * outNormals)
{
    //Go through every group of three triangles and compute the normal.
    Vector3f cross1, cross2, crossed;
    for (int i = 0; i + 2 < nVertices; i += 3)
    {
        //Get two vectors to cross.
        cross1 = (vertices[i] - vertices[i + 1]).Normalized();
        cross2 = (vertices[i] - vertices[i + 2]).Normalized();
        crossed = cross1.Cross(cross2).Normalized();

        //Set the normals to that value.
        outNormals[i] = crossed;
        outNormals[i + 1] = crossed;
        outNormals[i + 2] = crossed;
    }
}