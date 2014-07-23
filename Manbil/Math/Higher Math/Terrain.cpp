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
													    Vector2f(baseLDelta2.x, baseLDelta2.y), operator[](baseL + baseLDelta2),
													    Vector2f(p.x - baseL.x, p.y - baseL.y));
}

void Terrain::GenerateVerticesIndices(std::vector<TerrainVertex> & outVertices, std::vector<unsigned int> & outIndices,
                                      Vector2u topLeft, Vector2u bottomRight, float heightScale, unsigned int zoomOut) const
{
    assert(topLeft.x <= bottomRight.x && topLeft.y <= bottomRight.y);

    topLeft = topLeft.Clamp(0, GetSize() - 1);
    bottomRight = bottomRight.Clamp(0, GetSize() - 1);

    outVertices.reserve(GetVerticesCount(zoomOut));
    outIndices.reserve(GetIndicesCount(zoomOut));

    Vector2u areaSize = bottomRight - topLeft + Vector2u(1, 1);

    float texCoordIncrement = 1.0f / (float)GetSize();
    unsigned int vertIndex = 0;
    Vector2f posF;
    for (Vector2u pos(0, topLeft.y); pos.y <= bottomRight.y; ++pos.y)
    {
        posF.y = (float)pos.y;

        for (pos.x = topLeft.x; pos.x <= bottomRight.x; ++pos.x)
        {
            posF.x = (float)pos.x;

            outVertices.insert(outVertices.end(), TerrainVertex());

            //Output position and texture coordinate.
            outVertices[vertIndex].Pos = Vector3f(posF.x, posF.y, heightScale * heightmap[pos]);
            outVertices[vertIndex].TexCoords = Vector2f(texCoordIncrement * posF.x, texCoordIncrement * posF.y);

            //Calculate normals and indices if this vertex isn't on the top/left borders.
            if (pos.x > 0 && pos.y > 0)
            {
                Vector3f lessX = outVertices[vertIndex - 1].Pos,
                         lessY = outVertices[vertIndex - areaSize.x].Pos,
                         toLessX = (lessX - outVertices[vertIndex].Pos).Normalized(),
                         toLessY = (lessY - outVertices[vertIndex].Pos).Normalized();
                outVertices[vertIndex].Normal = toLessX.Cross(toLessY);
                assert(outVertices[vertIndex].Normal.z * heightScale > 0.0f);


                outIndices.insert(outIndices.end(), vertIndex);
                outIndices.insert(outIndices.end(), vertIndex - 1);
                outIndices.insert(outIndices.end(), vertIndex - 1 - areaSize.x);

                outIndices.insert(outIndices.end(), vertIndex);
                outIndices.insert(outIndices.end(), vertIndex - 1 - areaSize.x);
                outIndices.insert(outIndices.end(), vertIndex - areaSize.x);
            }

            vertIndex += 1;
        }
    }

    //Calculate the normals that couldn't be calculated normally (the ones on the top/left borders).

    //First, the top-left spot.
    Vector3f moreX = outVertices[1].Pos,
             moreY = outVertices[GetSize()].Pos,
             toMoreX = (moreX - outVertices[0].Pos).Normalized(),
             toMoreY = (moreY - outVertices[0].Pos).Normalized();
    outVertices[0].Normal = toMoreX.Cross(toMoreY);
    assert(outVertices[0].Normal.z * heightScale > 0.0f);

    for (unsigned int i = 1; i < GetSize(); ++i)
    {
        //Left border.

        vertIndex = i * areaSize.x;
        
        moreX = outVertices[vertIndex + 1].Pos;
        Vector3f lessY = outVertices[vertIndex - areaSize.x].Pos;
        toMoreX = (moreX - outVertices[vertIndex].Pos).Normalized();
        Vector3f toLessY = (lessY - outVertices[vertIndex].Pos).Normalized();

        outVertices[vertIndex].Normal = toLessY.Cross(toMoreX);
        assert(outVertices[vertIndex].Normal.z * heightScale > 0.0f);


        //Top border.

        vertIndex = i;

        Vector3f lessX = outVertices[vertIndex - 1].Pos;
        moreY = outVertices[vertIndex + areaSize.x].Pos;
        Vector3f toLessX = (lessX - outVertices[vertIndex].Pos).Normalized();
        toMoreY = (moreY - outVertices[vertIndex].Pos).Normalized();

        outVertices[vertIndex].Normal = toMoreY.Cross(toLessX);
        assert(outVertices[vertIndex].Normal.z * heightScale > 0.0f);
    }
}