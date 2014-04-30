#include "Terrain.h"

#include <iostream>

#include "GeometricMath.h"

unsigned char Terrain::Exception_Invalid_Area = 1;

void Terrain::SetHeightmap(const Array2D<float> & copy)
{
	//If the dimensions are different, make a new heightmap object.
	if (copy.GetWidth() != heightmap->GetWidth() || copy.GetHeight() != heightmap->GetHeight())
	{
		delete heightmap;
		heightmap = new Array2D<float>(copy.GetWidth(), copy.GetHeight(), 0.0f);
	}

	//Add in the new heightmap values.
	Vector2i l;
	for (int x = 0; x < copy.GetWidth(); ++x)
	{
		l.x = x;

		for (int y = 0; y < copy.GetHeight(); ++y)
		{
			l.y = y;

			(*heightmap)[l] = copy[l];
		}
	}
}

float Terrain::Interp(Vector2f p) const
{
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

	Vector2i baseL((int)p.x, (int)p.y),
			   baseLDelta1(1, 1),
			   baseLDelta2;

	float correspondingY = p.x - (int)p.x;

	if ((p.y - (int)p.y) < correspondingY)
	{
		baseLDelta2 = Vector2i(1, 0);
	}
	else
	{
		baseLDelta2 = Vector2i(0, 1);
	}

	//Interpolate.
	return GeometricMath::TriangleInterpolate<Vector2f>(Vector2f(0, 0), operator[](baseL),
													  Vector2f(1, 1), operator[](baseL + baseLDelta1),
													  baseLDelta2,	  operator[](baseL + baseLDelta2),
													  p - baseL);
}


Vector2i Constrain(Vector2i value, Vector2i max, Vector2i min = Vector2i())
{
	if (value.x < min.x) value.x = min.x;
	if (value.y < min.y) value.y = min.y;

	if (value.x > max.x) value.x = max.x;
	if (value.y > max.y) value.y = max.y;

	return value;
}

void Terrain::CreateVertexPositions(Vector3f * outPositions, Vector2i topLeft, Vector2i bottomRight, int zoomOut) const
{
	//TODO: Handle zoomOut parameter.

	if (topLeft.x > bottomRight.x || topLeft.y > bottomRight.y)
	{
		throw Exception_Invalid_Area;
	}
	topLeft = Constrain(topLeft, Vector2i(GetSize()-1, GetSize()-1));
	bottomRight = Constrain(bottomRight, Vector2i(GetSize()-1, GetSize()-1));


	int pX, pY;
	Vector2i loc;

	for (pY = topLeft.y; pY <= bottomRight.y; ++pY)
	{
		loc.y = pY;

		for (pX = topLeft.x; pX <= bottomRight.x; ++pX)
		{
			loc.x = pX;

			outPositions[GetIndex(loc, topLeft, bottomRight)] = Vector3f(pX, pY, (*heightmap)[loc]);
		}
	}
}
void Terrain::CreateVertexIndices(unsigned int * outIndices, Vector2i topLeft, Vector2i bottomRight, int zoomOut) const
{
	//TODO: Handle zoomOut parameter.

	if (topLeft.x > bottomRight.x || topLeft.y > bottomRight.y)
	{
		throw Exception_Invalid_Area;
	}
    topLeft = Constrain(topLeft, Vector2i(GetSize() - 1, GetSize() - 1));
    bottomRight = Constrain(bottomRight, Vector2i(GetSize() - 1, GetSize() - 1));


	unsigned int indexIndex = 0;
	Vector2i loc;

	unsigned int pX, pY, tl, tr, bl, br;
	for (pY = topLeft.y; pY < bottomRight.y; ++pY)
	{
		loc.y = pY;

		for (pX = topLeft.x; pX < bottomRight.x; ++pX)
		{
			loc.x = pX;

            tl = GetIndex(loc, topLeft, bottomRight);
			tr = tl + 1;
            bl = GetIndex(loc + Vector2i(0, 1), topLeft, bottomRight);
			br = bl + 1;

			outIndices[indexIndex++] = br;
			outIndices[indexIndex++] = bl;
			outIndices[indexIndex++] = tl;

			outIndices[indexIndex++] = br;
			outIndices[indexIndex++] = tl;
            outIndices[indexIndex++] = tr;
		}
	}
}
void Terrain::CreateVertexNormals(Vector3f * outNormals, const Vector3f * const vertexPositions, Vector3f vPosScale, Vector2i topLeft, Vector2i bottomRight, int zoomOut) const
{
	//TODO: Handle zoomOut parameter.

	if (topLeft.x > bottomRight.x || topLeft.y > bottomRight.y)
	{
		throw Exception_Invalid_Area;
	}
    topLeft = Constrain(topLeft, Vector2i(GetSize() - 1, GetSize() - 1));
    bottomRight = Constrain(bottomRight, Vector2i(GetSize() - 1, GetSize() - 1));


	int tl, tm, tr, l, m, r, bl, bm, br;
	Vector3f rV, lV, tmV, bmV,
			 mM;
	Vector2i loc;

	int normals;
	Vector3f normal, tempNormal;

	bool pastLeft, beforeRight, pastTop, beforeBottom;

	int pX, pY;
	for (pY = topLeft.y; pY <= bottomRight.y; ++pY)
	{
		loc.y = pY;

		for (pX = topLeft.x; pX <= bottomRight.x; ++pX)
		{
			loc.x = pX;

			//Get the indices for the 3x3 square surrounding the position.

            tl = GetIndex(loc + Vector2i(-1, -1), topLeft, bottomRight);
			tm = tl + 1;
			tr = tm + 1;

            l = GetIndex(loc + Vector2i(-1, 0), topLeft, bottomRight);
			m = l + 1;
			r = m + 1;

            bl = GetIndex(loc + Vector2i(-1, 1), topLeft, bottomRight);
			bm = bl + 1;
			br = bm + 1;


			//Some pre-computation.

			pastLeft = (pX > 0);
			beforeRight = (pX < heightmap->GetWidth() - 1);
			pastTop = (pY > 0);
			beforeBottom = (pY < heightmap->GetHeight() - 1);

			mM = vertexPositions[m].ComponentProduct(vPosScale);
			if (pastLeft)
			{
				lV = (mM - vertexPositions[l].ComponentProduct(vPosScale));
			}
			if (beforeRight)
			{
				rV = (mM - vertexPositions[r].ComponentProduct(vPosScale));
			}
			if (pastTop)
			{
				tmV = (mM - vertexPositions[tm].ComponentProduct(vPosScale));
			}
			if (beforeBottom)
			{
				bmV = (mM - vertexPositions[bm].ComponentProduct(vPosScale));
			}


			//Average the normals of the surfaces surrounding the position.

			normals = 0;
			normal = Vector3f(0, 0, 0);

			if (pastLeft)
			{
				if (pastTop)
				{
					normals += 1;

					tempNormal = lV.Cross(tmV).Normalized();
					if (tempNormal.z < 0.0f)
					{
						tempNormal = -tempNormal;
					}

					normal += tempNormal;
				}
				if (beforeBottom)
				{
					normals += 1;

					tempNormal = (lV).Cross(bmV).Normalized();
					if (tempNormal.z < 0.0f)
					{
						tempNormal = -tempNormal;
					}

					normal += tempNormal;
				}
			}
			if (beforeRight)
			{
				if (pastTop)
				{
					normals += 1;

					tempNormal = rV.Cross(tmV).Normalized();
					if (tempNormal.z < 0.0f)
					{
						tempNormal = -tempNormal;
					}

					normal += tempNormal;
				}
				if (beforeBottom)
				{
					normals += 1;

					tempNormal = rV.Cross(bmV).Normalized();
					if (tempNormal.z < 0.0f)
					{
						tempNormal = -tempNormal;
					}

					normal += tempNormal;
				}
			}

			if (normals > 0)
			{
				normal /= (float)normals;
				normal.Normalize();
			}
			else
			{
				normal = Vector3f(0, 0, 1);
			}


			//Set the normal for the position.
            outNormals[GetIndex(loc, topLeft, bottomRight)] = normal;
		}
	}
}
void Terrain::CreateVertexTexCoords(Vector2f * texCoords, Vector2i topLeft, Vector2i bottomRight, int zoomOut) const
{
	//TODO: Handle zoomOut parameter.

	if (topLeft.x > bottomRight.x || topLeft.y > bottomRight.y)
	{
		throw Exception_Invalid_Area;
	}
    topLeft = Constrain(topLeft, Vector2i(GetSize() - 1, GetSize() - 1));
    bottomRight = Constrain(bottomRight, Vector2i(GetSize() - 1, GetSize() - 1));


    float increment = 1.0f / GetSize();
    for (int x = topLeft.x; x <= bottomRight.x; ++x)
        for (int y = topLeft.y; y <= bottomRight.y; ++y)
        {
            texCoords[GetIndex(Vector2i(x, y), topLeft, bottomRight)] = Vector2f(increment * x, increment * y);
        }
}