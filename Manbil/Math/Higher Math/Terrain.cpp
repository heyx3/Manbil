#include "Terrain.h"

#include "Geometryf.h"

#include <iostream>
#include <assert.h>


Vector2u Terrain::GetNVertices(Vector2u regionSize, unsigned int zoomOut)
{
    Vector2u outSize = regionSize;
    for (unsigned int i = 0; i < zoomOut; ++i)
    {
        if (outSize.x % 2 == 0 || outSize.x < 2 ||
            outSize.y % 2 == 0 || outSize.y < 2)
        {
            return Vector2u(0, 0);
        }

        outSize = (outSize / 2) + Vector2u(1, 1);
    }
    return outSize;
}
//Gets the number of indices necessary to generate triangles for a terrain of the given size.
//Takes in the LOD (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.).
//Returns 0 if the given LOD cannot be applied to the given region size.
unsigned int Terrain::GetNIndices(Vector2u regionSize, unsigned int zoomOut)
{
    Vector2u nVerts = GetNVertices(regionSize, zoomOut);
    if (nVerts.x < 2 || nVerts.y < 2)
    {
        return 0;
    }
    return (6 * (nVerts.x - 1) * (nVerts.y - 1));
}

void Terrain::SetHeightmap(const Array2D<float> & copy)
{
    heightmap.Reset(copy.GetWidth(), copy.GetHeight());
    heightmap.Fill(copy.GetArray(), true);
}

float Terrain::Interp(Vector2f p) const
{
    p = p.Clamp(Vector2f(0.0f, 0.0f),
                Vector2f((float)GetWidth(), (float)GetHeight()));

	//There are two triangles in the terrain square:
	// (x, y) - (x + 1, y) - (x + 1, y + 1)
	// (x, y) - (x, y + 1) - (x + 1, y + 1)

	//(x, y)
	//.-------. (x + 1, y)
	//|\      |
	//| \     |
	//|  \    |
	//|   \   |
	//|    \  |
	//|     \ |
	//|      \|
	//.-------\. (x + 1, y + 1)
	//(x, y + 1) 


	//Figure out which triangle to use.

	Vector2u baseL((unsigned int)p.x, (unsigned int)p.y),
			 baseLDelta1(1, 1),
			 baseLDelta2;

	float correspondingY = p.x - (unsigned int)p.x;

	if ((p.y - (unsigned int)p.y) < correspondingY)
	{
		baseLDelta2 = Vector2u(1, 0);
	}
	else
	{
		baseLDelta2 = Vector2u(0, 1);
	}

	//Interpolate.
	return Geometryf::TriangleInterpolate<Vector2f>(Vector2f(0, 0), operator[](baseL),
													Vector2f(1, 1), operator[](baseL + baseLDelta1),
													Vector2f(baseLDelta2.x, baseLDelta2.y),
                                                    operator[](baseL + baseLDelta2),
													Vector2f(p.x - baseL.x, p.y - baseL.y));
}