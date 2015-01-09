#include "Geometryf.h"


void Geometryf::CalculateNormals(void* verts, unsigned int sizeofVerts, unsigned int nVerts,
                                 const unsigned int * inds, unsigned int nInds,
                                 Vector3f(*getNormal)(const void* vertex),
                                 void(*setNormal)(void * vertex, Vector3f normal),
                                 Vector3f(*getPos)(const void* vertex),
                                 bool(*shouldFlipNormal)(Vector3f normal, void* vertex, void* pData),
                                 void* pData)
{
    unsigned char* vertsB = (unsigned char*)verts;

    //Start all normals at 0.
    for (unsigned int vert = 0; vert < nVerts; ++vert)
        setNormal(&vertsB[vert * sizeofVerts], Vector3f());

    //Get the normals for each triangle.
    for (unsigned int tri = 0; tri < nInds; tri += 3)
    {
        void *v1 = &vertsB[inds[tri] * sizeofVerts],
             *v2 = &vertsB[inds[(tri + 1)] * sizeofVerts],
             *v3 = &vertsB[inds[(tri + 2)] * sizeofVerts];
        Vector3f v_1_2 = getPos(v1) - getPos(v2),
                 v_1_3 = getPos(v1) - getPos(v3);
        Vector3f norm = v_1_2.Normalized().Cross(v_1_3.Normalized());

        if (shouldFlipNormal(norm, v1, pData)) norm = -norm;

        setNormal(v1, norm + getNormal(v1));
        setNormal(v2, norm + getNormal(v2));
        setNormal(v3, norm + getNormal(v3));
    }

    //Get the average of each vertex's normals.
    for (unsigned int vert = 0; vert < nVerts; ++vert)
        setNormal(&vertsB[vert * sizeofVerts],
                  getNormal(&vertsB[vert * sizeofVerts]).Normalized());
}