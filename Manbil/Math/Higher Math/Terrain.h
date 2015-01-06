#pragma once

#include "../LowerMath.hpp"
#include "../../Vertices.h"
#include <vector>


//Represents a heightmapped terrain using floats.
//Must be a power of two because of the LOD system.
class Terrain
{
public:

    //Gets the number of vertices necessary to generate triangles for a terrain of the given size.
    //Assumes the size of the region is a power of 2.
    //Takes in the LOD (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.).
    static inline unsigned int GetVerticesCount(unsigned int regionSize, unsigned int zoomOut = 0)
    {
        return (regionSize * regionSize) / (unsigned int)BasicMath::IntPow(4, zoomOut);
    }
    //Gets the number of indices necessary to generate triangles for a terrain of the given size.
    //Assumes the size of the region is a power of 2.
    //Takes in the LOD (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.).
    static inline unsigned int GetIndicesCount(unsigned int regionSize, unsigned int zoomOut = 0)
    {
        return (6 * (regionSize - 1) * (regionSize - 1)) / (unsigned int)BasicMath::IntPow(4, zoomOut);
    }


    Terrain(unsigned int size) : heightmap(size, size, 0.0f) { }

    //Gets the width/height of the terrain.
	unsigned int GetSize(void) const { return heightmap.GetWidth(); }


	void SetHeightmap(const Array2D<float>& copy);

	float& operator[](Vector2u l) { return heightmap[l]; }
	const float& operator[](Vector2u l) const { return heightmap[l]; }

	float operator[](Vector2f p) const { return Interp(p); }


    //"VertexType" is the class of vertex. It must have a default constructor.
    //"VertexSetPosFunc" is a function like
    //    "void SetPos(VertexType& v, Vector3f pos)".
    //"VertexGetPosFunc" is a function like "Vector3f GetPos(const VertexType& vertex)".
    //"VertexSetUVFunc" is a function like
    //    "void SetUV(VertexType& v, Vector2f uv)".
    //"VertexGetUVFunc" is a function like "Vector2f GetUV(const VertexType& vertex)".
    //"VertexSetNormalFunc" is a function like
    //    "void SetNormal(VertexType& v, Vector3f normal)".
    //"VertexGetNormalFunc" is a function like "Vector3f GetNormal(const VertexType& vertex)".
    template<typename VertexType, typename VertexSetPosFunc, typename VertexGetPosFunc,
             typename VertexSetUVFunc, typename VertexGetUVFunc,
             typename VertexSetNormalFunc, typename VertexGetNormalFunc>
    //Generates positions and indices for some rectangular subset of this terrain.
    //Takes in:
    //    1) The output collections for vertices/indices.
    //    2) Getters/setters for a vertex's position.
    //    3) The rectangular region of terrain to create.
    //    4) The scale for the terrain's height
    //    5) The LOD level of the terrain (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.)
    //Assumes the size of the rectangular region is a power of 2.
    void GenerateTriangles(std::vector<VertexType>& outVerts, std::vector<unsigned int>& outIndices,
                           VertexSetPosFunc vertPosSetter, VertexGetPosFunc vertPosGetter,
                           VertexSetUVFunc vertUVSetter, VertexGetUVFunc vertUVGetter,
                           VertexSetNormalFunc vertNormalSetter, VertexGetNormalFunc vertNormalGetter,
                           Vector2u topLeft, Vector2u bottomRight,
                           float heightScale = 1.0f, unsigned int zoomOut = 0) const
    {
        assert(topLeft.x <= bottomRight.x && topLeft.y <= bottomRight.y);

        topLeft = topLeft.Clamp(0, GetSize() - 1);
        bottomRight = bottomRight.Clamp(0, GetSize() - 1);

        Vector2u areaSize = bottomRight - topLeft + Vector2u(1, 1);

        float texCoordIncrement = 1.0f / (float)GetSize();
        unsigned int vertIndex = outVerts.size();
        Vector2f posF;
        for (Vector2u pos(0, topLeft.y); pos.y <= bottomRight.y; ++pos.y)
        {
            posF.y = (float)pos.y;

            for (pos.x = topLeft.x; pos.x <= bottomRight.x; ++pos.x)
            {
                posF.x = (float)pos.x;

                outVerts.insert(outVerts.end(), VertexType());

                //Output position and texture coordinates.
                vertPosSetter(outVerts[vertIndex], Vector3f(posF.x, posF.y, heightScale * heightmap[pos]));
                vertUVSetter(outVerts[vertIndex], Vector2f(texCoordIncrement * posF.x, texCoordIncrement * posF.y));

                //Calculate normals and indices if this vertex isn't on the top/left borders.
                if (pos.x > 0 && pos.y > 0)
                {
                    Vector3f lessX = vertPosGetter(outVerts[vertIndex - 1]),
                             lessY = vertPosGetter(outVerts[vertIndex - areaSize.x]),
                             toLessX = (lessX - vertPosGetter(outVerts[vertIndex])).Normalized(),
                             toLessY = (lessY - vertPosGetter(outVerts[vertIndex])).Normalized();
                    vertNormalSetter(outVerts[vertIndex], toLessX.Cross(toLessY));
                    assert(vertNormalGetter(outVerts[vertIndex]).z * heightScale > 0.0f);


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
        Vector3f moreX = vertPosGetter(outVerts[1]),
                 moreY = vertPosGetter(outVerts[GetSize()]),
                 toMoreX = (moreX - vertPosGetter(outVerts[0])).Normalized(),
                 toMoreY = (moreY - vertPosGetter(outVerts[0])).Normalized();
        vertNormalGetter(outVerts[0]) = toMoreX.Cross(toMoreY);
        assert(vertNormalGetter(outVerts[0]).z * heightScale > 0.0f);

        for (unsigned int i = 1; i < GetSize(); ++i)
        {
            //Left border.

            vertIndex = i * areaSize.x;

            moreX = vertPosGetter(outVerts[vertIndex + 1]);
            Vector3f lessY = vertPosGetter(outVerts[vertIndex - areaSize.x]);
            toMoreX = (moreX - vertPosGetter(outVerts[vertIndex])).Normalized();
            Vector3f toLessY = (lessY - vertPosGetter(outVerts[vertIndex])).Normalized();

            vertNormalSetter(outVerts[vertIndex], toLessY.Cross(toMoreX));
            assert(vertNormalGetter(outVerts[vertIndex]).z * heightScale > 0.0f);


            //Top border.

            vertIndex = i;

            Vector3f lessX = vertPosGetter(outVerts[vertIndex - 1]);
            moreY = vertPosGetter(outVerts[vertIndex + areaSize.x]);
            Vector3f toLessX = (lessX - vertPosGetter(outVerts[vertIndex])).Normalized();
            toMoreY = (moreY - vertPosGetter(outVerts[vertIndex])).Normalized();

            vertNormalSetter(outVerts[vertIndex], toMoreY.Cross(toLessX));
            assert(vertNormalGetter(outVerts[vertIndex]).z * heightScale > 0.0f);
        }
    }
    //"VertexType" is the class of vertex. It must have a default constructor.
    //"VertexSetPosFunc" is a function like
    //    "void SetPos(VertexType& v, Vector3f pos)".
    //"VertexGetPosFunc" is a function like "Vector3f GetPos(const VertexType& vertex)".
    //"VertexSetUVFunc" is a function like
    //    "void SetUV(VertexType& v, Vector2f uv)".
    //"VertexGetUVFunc" is a function like "Vector2f GetUV(const VertexType& vertex)".
    //"VertexSetNormalFunc" is a function like
    //    "void SetNormal(VertexType& v, Vector3f normal)".
    //"VertexGetNormalFunc" is a function like "Vector3f GetNormal(const VertexType& vertex)".
    template<typename VertexType, typename VertexSetPosFunc, typename VertexGetPosFunc,
             typename VertexSetUVFunc, typename VertexGetUVFunc,
             typename VertexSetNormalFunc, typename VertexGetNormalFunc>
    //Generates positions and indices for this terrain.
    //Takes in:
    //    1) The output collections for vertices/indices.
    //    2) Getters/setters for a vertex's position.
    //    3) The scale for the terrain's height
    //    4) The LOD level of the terrain (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.)
    void GenerateTriangles(std::vector<VertexType>& outVerts, std::vector<unsigned int>& outIndices,
                           VertexSetPosFunc vertPosSetter, VertexGetPosFunc vertPosGetter,
                           VertexSetUVFunc vertUVSetter, VertexGetUVFunc vertUVGetter,
                           VertexSetNormalFunc vertNormSetter, VertexGetNormalFunc vertNormGetter,
                           float heightScale = 1.0f, unsigned int zoomOut = 0) const
    {
        GenerateTriangles(outVerts, outIndices, vertPosSetter, vertPosGetter, vertUVSetter, vertUVGetter,
                          vertNormSetter, vertNormGetter, Vector2u(), Vector2u(GetSize(), GetSize()),
                          heightScale, zoomOut);
    }


private:

	Array2D<float> heightmap;

	//Gets the height at the given fractional position using interpolation.
	float Interp(Vector2f pos) const;
};