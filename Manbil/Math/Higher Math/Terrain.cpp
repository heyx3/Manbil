#include "Terrain.h"

#include "GeometricMath.h"

#include <iostream>
#include <assert.h>



void Terrain::SetHeightmap(const Array2D<float> & copy)
{
    heightmap.Reset(copy.GetWidth(), copy.GetHeight());
    heightmap.Fill(copy.GetArray(), true);
}

float Terrain::Interp(Vector2f p) const
{
    p = p.Clamp(0.0f, (float)GetSize());

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
	return GeometricMath::TriangleInterpolate<Vector2f>(Vector2f(0, 0), operator[](baseL),
													    Vector2f(1, 1), operator[](baseL + baseLDelta1),
													    Vector2f(baseLDelta2.x, baseLDelta2.y),
                                                        operator[](baseL + baseLDelta2),
													    Vector2f(p.x - baseL.x, p.y - baseL.y));
}