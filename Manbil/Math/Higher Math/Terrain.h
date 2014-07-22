#pragma once

#include "../LowerMath.hpp"
#include "../../Vertices.h"
#include <vector>


//Represents a heightmap terrain using floats.
//TODO: Refactor this class's handling of heightmaps so that it takes a heightmap pointer instead of managing its own heightmap.
class Terrain
{
public:

    Terrain(unsigned int size) : heightmap(size, size, 0.0f) { }

    //Gets the width/height of the terrain.
	unsigned int GetSize(void) const { return heightmap.GetWidth(); }

	//Gets the number of vertices necessary to turn this terrain into triangles.
	unsigned int GetVerticesCount(unsigned int zoomOut = 0) const
    {
        return (GetSize() * GetSize()) / (unsigned int)BasicMath::IntPow(4, zoomOut);
    }
	//Gets the number of indices necessary to turn this terrain into triangles.
	unsigned int GetIndicesCount(unsigned int zoomOut = 0) const
    {
        return (6 * (GetSize() - 1) * (GetSize() - 1)) / (unsigned int)BasicMath::IntPow(4, zoomOut);
    }

	void SetHeightmap(const Array2D<float> & copy);

	float& operator[](Vector2u l) { return heightmap[l]; }
	const float& operator[](Vector2u l) const { return heightmap[l]; }

	float operator[](Vector2f p) const { return Interp(p); }

    typedef VertexPosTex1Normal TerrainVertex;

    void GenerateVerticesIndices(std::vector<TerrainVertex> & outVertices, std::vector<unsigned int> & outIndices,
                                 Vector2u topLeft, Vector2u bottomRight, float heightScale = 1.0f, unsigned int zoomOut = 0) const;
    void GenerateVerticesIndices(std::vector<TerrainVertex> & outVertices, std::vector<unsigned int> & outIndices, float heightScale = 1.0f, unsigned int zoomOut = 0) const
    {
        GenerateVerticesIndices(outVertices, outIndices, Vector2u(), Vector2u(GetSize(), GetSize()), heightScale, zoomOut);
    }


private:

	Array2D<float> heightmap;

	//Gets the height at the given fractional position using interpolation.
	float Interp(Vector2f pos) const;
};