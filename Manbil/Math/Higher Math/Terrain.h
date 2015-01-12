#pragma once

#include "../LowerMath.hpp"
#include "../../Vertices.h"
#include <vector>


//Represents a rectangular terrain with a heightmap of floats.
//Must have an odd number of heightmap elements if using the LOD system
//    (preferably one more than a power of two).
class Terrain
{
public:

    //Gets the number of vertices needed along each axis to generate triangles
    //    for a terrain of the given size.
    //Takes in the LOD (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.).
    //Returns {0, 0} if the given LOD cannot be applied to the given region size.
    static Vector2u GetNVertices(Vector2u regionSize, unsigned int zoomOut = 0)
    {
        Vector2u outSize = regionSize;
        for (unsigned int i = 0; i < zoomOut; ++i)
        {
            if (outSize.x % 2 == 0 || outSize.x < 2 ||
                outSize.y % 2 == 0 || outSize.y < 2)
                return Vector2u(0, 0);

            outSize = (outSize / 2) + Vector2u(1, 1);
        }
        return outSize;
    }
    //Gets the number of indices necessary to generate triangles for a terrain of the given size.
    //Takes in the LOD (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.).
    //Returns 0 if the given LOD cannot be applied to the given region size.
    static unsigned int GetNIndices(Vector2u regionSize, unsigned int zoomOut = 0)
    {
        Vector2u nVerts = GetNVertices(regionSize, zoomOut);
        if (nVerts.x < 2 || nVerts.y < 2) return 0;
        return (6 * (nVerts.x - 1) * (nVerts.y - 1));
    }


    Terrain(Vector2u size) : heightmap(size.x, size.y, 0.0f) { }


    unsigned int GetWidth(void) const { return heightmap.GetWidth(); }
    unsigned int GetHeight(void) const { return heightmap.GetHeight(); }


	void SetHeightmap(const Array2D<float>& copy);

	float& operator[](Vector2u l) { return heightmap[l]; }
	const float& operator[](Vector2u l) const { return heightmap[l]; }

	float operator[](Vector2f p) const { return Interp(p); }


    //"VertexType" is the class of vertex. It must have a default constructor.
    template<typename VertexType>
    //Generates positions and indices for some rectangular subset of this terrain.
    //Takes in:
    //    1) The output collections for vertices/indices.
    //    2) Getters for a vertex's data (pass 0 for the vertex normal getter to not compute normals).
    //    3) The rectangular region of terrain to create.
    //    4) The scale for the terrain's height
    //    5) The LOD level of the terrain (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.)
    //Assumes the region can be split down to the given level of detail.
    void GenerateTriangles(std::vector<VertexType>& outVerts, std::vector<unsigned int>& outIndices,
                           Vector3f&(*vertPosGetter)(VertexType& vert),
                           Vector2f&(*vertUVGetter)(VertexType& vert),
                           Vector3f&(*vertNormalGetter)(VertexType& vert),
                           Vector2u topLeft, Vector2u bottomRight,
                           float heightScale = 1.0f, unsigned int zoomOut = 0) const
    {
        assert(outVerts.size() == 0);
        assert(outIndices.size() == 0);

        //If the LOD level isn't 0, calculate a lower-detail terrain and generate triangles for it.
        if (zoomOut != 0)
        {
            Vector2u fullSize = bottomRight - topLeft;
            unsigned int skippedVerts = Mathf::IntPow(2, zoomOut) - 1;
            
            //Calculate the size of the lower-detail terrain and make sure it can be constructed.
            Vector2u outSize = GetNVertices(fullSize, zoomOut);
            assert(outSize.x > 1 && outSize.y > 1);

            
            //Fill in the lower-detail heightmap.

            Terrain smallerTerr(outSize);
            Vector2u lodLoc(0, 0);
            Vector2u terrainLoc = topLeft;

            while (terrainLoc.y <= bottomRight.y)
            {
                terrainLoc.x = topLeft.x;
                lodLoc.x = 0;

                while (terrainLoc.x <= bottomRight.x)
                {
                    smallerTerr[lodLoc] = heightmap[terrainLoc];

                    terrainLoc.x += skippedVerts + 1;
                    lodLoc.x += 1;
                }

                terrainLoc.y += skippedVerts + 1;
                lodLoc.y += 1;
            }


            //Generate the triangles.
            float scalePos = powf(2.0f, (float)skippedVerts);
            Vector3f translatePos(ToV2f(topLeft), 0.0f);
            unsigned int startIndex = outVerts.size();
            smallerTerr.GenerateTriangles(outVerts, outIndices,
                                          vertPosGetter, vertUVGetter, vertNormalGetter,
                                          Vector2u(0, 0),
                                          Vector2u(smallerTerr.GetWidth(), smallerTerr.GetHeight()),
                                          1.0f / scalePos);

            //Remap the vertices to fit the correct region of space.
            Vector3f scalePos3(scalePos, scalePos, 1.0f);
            for (unsigned int i = startIndex; i < outVerts.size(); ++i)
            {
                Vector3f& vPos = vertPosGetter(outVerts[i]);
                vPos.MultiplyComponents(scalePos3);
                vPos += translatePos;
            }

            return;
        }


        //Otherwise, level of detail is 0 (full), so generate the terrain like normal.
        assert(topLeft.x <= bottomRight.x && topLeft.y <= bottomRight.y);

        topLeft = topLeft.Clamp(Vector2u(0, 0),
                                Vector2u(GetWidth() - 1, GetHeight() - 1));
        bottomRight = bottomRight.Clamp(Vector2u(0, 0),
                                        Vector2u(GetWidth() - 1, GetHeight() - 1));

        Vector2u areaSize = bottomRight - topLeft + Vector2u(1, 1);

        Vector2f texCoordIncrement(1.0f / (float)GetWidth(),
                                   1.0f / (float)GetHeight());
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
                vertPosGetter(outVerts[vertIndex]) = Vector3f(posF.x, posF.y,
                                                              heightScale * heightmap[pos]);
                vertUVGetter(outVerts[vertIndex]) = Vector2f(texCoordIncrement.x * posF.x,
                                                             texCoordIncrement.y * posF.y);

                //Calculate normals and indices if this vertex isn't on the top/left borders.
                if (pos.x > 0 && pos.y > 0)
                {
                    if (vertNormalGetter != 0)
                    {
                        Vector3f lessX = vertPosGetter(outVerts[vertIndex - 1]),
                                 lessY = vertPosGetter(outVerts[vertIndex - areaSize.x]),
                                 toLessX = (lessX - vertPosGetter(outVerts[vertIndex])).Normalized(),
                                 toLessY = (lessY - vertPosGetter(outVerts[vertIndex])).Normalized();
                        Vector3f& vNorm = vertNormalGetter(outVerts[vertIndex]);
                        vNorm = toLessX.Cross(toLessY);
                        if (vNorm.z < 0.0f)
                            vNorm = -vNorm;
                    }

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

        if (vertNormalGetter == 0)
            return;

        //First, the top-left corner.
        if (topLeft.x == 0 && topLeft.y == 0)
        {
            Vector3f moreX = vertPosGetter(outVerts[1]),
                     moreY = vertPosGetter(outVerts[GetWidth()]),
                     toMoreX = (moreX - vertPosGetter(outVerts[0])).Normalized(),
                     toMoreY = (moreY - vertPosGetter(outVerts[0])).Normalized();
            Vector3f& vNorm = vertNormalGetter(outVerts[0]);
            vNorm = toMoreX.Cross(toMoreY);
            if (vNorm.z < 0.0f)
                vNorm = -vNorm;
        }

        //Next, the left edge.
        if (topLeft.x == 0)
        {
            for (unsigned int y = Mathf::Max<unsigned int>(topLeft.y, 1); y <= bottomRight.y; ++y)
            {
                vertIndex = y * areaSize.x;

                Vector3f moreX = vertPosGetter(outVerts[vertIndex + 1]),
                         lessY = vertPosGetter(outVerts[vertIndex - areaSize.x]),
                         toMoreX = (moreX - vertPosGetter(outVerts[vertIndex])).Normalized(),
                         toLessY = (lessY - vertPosGetter(outVerts[vertIndex])).Normalized();
                
                Vector3f& vNorm = vertNormalGetter(outVerts[vertIndex]);
                vNorm = toLessY.Cross(toMoreX);
                if (vNorm.z < 0.0f)
                    vNorm = -vNorm;
            }
        }

        //Finally, the top edge.
        if (topLeft.y == 0)
        {
            for (unsigned int x = Mathf::Max<unsigned int>(topLeft.x, 1); x <= bottomRight.x; ++x)
            {
                vertIndex = x;

                Vector3f lessX = vertPosGetter(outVerts[vertIndex - 1]),
                         moreY = vertPosGetter(outVerts[vertIndex + areaSize.x]),
                         toLessX = (lessX - vertPosGetter(outVerts[vertIndex])).Normalized(),
                         toMoreY = (moreY - vertPosGetter(outVerts[vertIndex])).Normalized();
                
                Vector3f& vNorm = vertNormalGetter(outVerts[vertIndex]);
                vNorm = toMoreY.Cross(toLessX);
                if (vNorm.z < 0.0f)
                    vNorm = -vNorm;
            }
        }
    }
    //"VertexType" is the class of vertex. It must have a default constructor.
    template<typename VertexType>
    //Generates positions and indices for this terrain.
    //Takes in:
    //    1) The output collections for vertices/indices.
    //    2) Getters for a vertex's data (pass 0 for the vertex normal getter to not compute normals).
    //    3) The scale for the terrain's height
    //    4) The LOD level of the terrain (0 = full detail, 1 = 1/4 detail, 2 = 1/8 detail, etc.)
    void GenerateTrianglesFull(std::vector<VertexType>& outVerts, std::vector<unsigned int>& outIndices,
                               Vector3f&(*vertPosGetter)(VertexType& vert),
                               Vector2f&(*vertUVGetter)(VertexType& vert),
                               Vector3f&(*vertNormalGetter)(VertexType& vert) = 0,
                               float heightScale = 1.0f, unsigned int zoomOut = 0) const
    {
        GenerateTriangles(outVerts, outIndices, vertPosGetter, vertUVGetter, vertNormalGetter,
                          Vector2u(), Vector2u(GetWidth() - 1, GetHeight() - 1), heightScale, zoomOut);
    }


private:

	Array2D<float> heightmap;

	//Gets the height at the given fractional position using interpolation.
	float Interp(Vector2f pos) const;
};