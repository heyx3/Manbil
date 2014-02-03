#pragma once

#include "../LowerMath.hpp"

//Represents a heightmap terrain using floats.
class Terrain
{
public:

    static bool DebugShit;

	Terrain(int size) { heightmap = new Fake2DArray<float>(size, size, 0.0f); }
	~Terrain(void) { delete heightmap; }

    //Gets the width/height of the terrain.
	int GetSize(void) const { return heightmap->GetWidth(); }

	//Gets the number of vertices necessary to turn this terrain into triangles.
	int GetVerticesCount(int zoomOut = 0) const { return (GetSize() * GetSize()) / (int)BasicMath::IntPow(4, zoomOut); }
	//Gets the number of indices necessary to turn this terrain into triangles.
	int GetIndicesCount(int zoomOut = 0) const { return (6 * (GetSize() - 1) * (GetSize() - 1)) / (int)BasicMath::IntPow(4, zoomOut); }

	void SetHeightmap(const Fake2DArray<float> & copy);

	float& operator[](Vector2i l) { return (*heightmap)[l]; }
	const float& operator[](Vector2i l) const { return (*heightmap)[l]; }

	const float operator[](Vector2f p) const { return Interp(p); }

	template<typename Func, class Out>
	//Takes a function of type "Out pConverter(Vector2i loc)" that converts a location in the terrain to output data.
	//Writes the converted values to a 2D array of type "Out".
	void ConvertHeightmap(Fake2DArray<Out> & outArray, Func pConverter)
	{
		Vector2i l;
		int x, y;
		for (x = 0; x < heightmap->GetWidth(); ++x)
		{
			l.x = x;

			for (y = 0; y < heightmap->GetHeight(); ++y)
			{
				l.y = y;

				outArray[l] = pConverter(l);
			}
		}
	}


	//The exception thrown when invalid top-left/bottom-right values are given for the following functions: "CreateVertexPositions", "CreateVertexIndices", "CreateVertexNormals", "CreateVertexTexCoords".
	static unsigned char Exception_Invalid_Area;

	//Creates vertex data for this terrain.
	//The "zoomOut" level describes how detailed it should be.

    void CreateVertexPositions(Vector3f * outPositions, Vector2i topLeft, Vector2i bottomRight, int zoomOut = 0) const;
    void CreateVertexIndices(unsigned int * outIndices, Vector2i topLeft, Vector2i bottomRight, int zoomOut = 0) const;
    void CreateVertexNormals(Vector3f * outNormals, const Vector3f * const vertexPositions, Vector3f vertexPosScale, Vector2i topLeft, Vector2i bottomRight, int zoomOut = 0) const;
    void CreateVertexTexCoords(Vector2f * texCoords, Vector2i topLeft, Vector2i bottomRight, int zoomOut = 0) const;

    void CreateVertexPositions(Vector3f * outPositions, int zoomOut = 0) const { CreateVertexPositions(outPositions, Vector2i(), Vector2i(GetSize(), GetSize()), zoomOut); }
    void CreateVertexIndices(unsigned int * outIndices, int zoomOut = 0) const { CreateVertexIndices(outIndices, Vector2i(), Vector2i(GetSize(), GetSize()), zoomOut); }
    void CreateVertexNormals(Vector3f * outNormals, const Vector3f * const vertexPositions, Vector3f vertexPosScale, int zoomOut = 0) const { CreateVertexNormals(outNormals, vertexPositions, vertexPosScale, Vector2i(), Vector2i(GetSize(), GetSize()), zoomOut); }
    void CreateVertexTexCoords(Vector2f * texCoords, int zoomOut = 0) const { CreateVertexTexCoords(texCoords, Vector2i(), Vector2i(GetSize(), GetSize()), zoomOut); }


private:

	int GetIndex(Vector2i pos, Vector2i topLeftOffset, Vector2i bottomRightOffset) const { return (pos.x - topLeftOffset.x) + ((pos.y - topLeftOffset.y) * (bottomRightOffset.x - topLeftOffset.x + 1)); }

	Fake2DArray<float> * heightmap;

	//Gets the height at the given fractional position using interpolation.
	float Interp(Vector2f pos) const;
};