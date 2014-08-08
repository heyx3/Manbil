#include "PlanetSimWorldGen.h"

#include "../../Math/Quaternion.h"
#include "../../DebugAssist.h"
#include <assert.h>



void WorldGen::GenerateVertices(const Array2D<float> & heightmap, Array2D<PlanetVertex> & outVertices,
                                float worldScale, float minHeight, float heightScale)
{
    assert(heightmap.GetArea() != 0);
    outVertices.Reset(heightmap.GetWidth(), heightmap.GetHeight());

    const float PI = 3.14159265f;
    const float verticalAngleScale = 0.999f;
    const float invMapWidth = 1.0f / (float)(heightmap.GetWidth() - 1),
                invMapHeight = 1.0f / (float)(heightmap.GetHeight() - 1);
    for (Vector2u loc; loc.y < heightmap.GetHeight(); ++loc.y)
    {
        for (loc.x = 0; loc.x < heightmap.GetWidth(); ++loc.x)
        {
            if (loc.x == 1 && loc.y == 255)
            {
                loc = loc;
            }

            //Calculate the rotation amounts needed to project this location onto a sphere.
            //Leave a tiny bit of room on the top/bottom for the extra top/bottom vertex.
            float verticalLerp = 1.0f - ((float)loc.y * invMapHeight);
            //verticalLerp = verticalLerp - (BasicMath::Smooth(verticalLerp) - verticalLerp); //(does the opposite of "Smooth")
            verticalLerp = BasicMath::Remap(0.0f, 1.0f, 1.0f - verticalAngleScale, verticalAngleScale, verticalLerp);
            float verticalAngle = PI * verticalLerp;
            float horizontalAngle = (2.0f * PI) * ((float)loc.x * invMapWidth);

            Quaternion rotVert(Vector3f(0.0f, 1.0f, 0.0f), verticalAngle);
            Quaternion rotHorz(Vector3f(0.0f, 0.0f, 1.0f), horizontalAngle);

            //Rotate vertically then horizontally.
            Vector3f pos = Vector3f(0.0f, 0.0f, -(minHeight + (heightmap[loc] * heightScale)));
            rotVert.Rotate(pos);
            rotHorz.Rotate(pos);
            pos *= worldScale;

            //Output vertex info.
            outVertices[loc].Pos = pos;
            outVertices[loc].Heightmap = heightmap[loc];

            //If there is a row and column before this one, calculate normals.
            if (loc.x > 0 && loc.y > 0)
            {
                Vector3f lessX = outVertices[loc.LessX()].Pos,
                         lessY = outVertices[loc.LessY()].Pos,
                         toLessX = (lessX - outVertices[loc].Pos).Normalized(),
                         toLessY = (lessY - outVertices[loc].Pos).Normalized();

                outVertices[loc].Normal = toLessY.Cross(toLessX);
                assert(outVertices[loc].Normal.Dot(-pos) < 0.0f);
            }
        }
    }


    //Generate the normals for the top/left borders.

    Vector3f moreX, moreY, lessX, lessY,
             toMoreX, toMoreY, toLessX, toLessY;

    //First, the top-left spot.
    Vector2u topLeft; assert(topLeft.x == 0 && topLeft.y == 0);
    moreX = outVertices[topLeft.MoreX()].Pos;
    moreY = outVertices[topLeft.MoreY()].Pos;
    toMoreX = (moreX - outVertices[topLeft].Pos).Normalized();
    toMoreY = (moreY - outVertices[topLeft].Pos).Normalized();
    outVertices[topLeft].Normal = toMoreY.Cross(toMoreX);
    assert(outVertices[topLeft].Normal.Dot(-outVertices[topLeft].Pos) < 0.0f);

    //Next, the top border.
    for (unsigned int x = 1; x < heightmap.GetWidth(); ++x)
    {
        Vector2u loc(x, 0);

        lessX = outVertices[loc.LessX()].Pos;
        moreY = outVertices[loc.MoreY()].Pos;
        toLessX = (lessX - outVertices[loc].Pos).Normalized();
        toMoreY = (moreY - outVertices[loc].Pos).Normalized();

        outVertices[loc].Normal = toLessX.Cross(toMoreY);
        assert(outVertices[loc].Normal.Dot(-outVertices[loc].Pos) < 0.0f);
    }
    //Finally, the left border.
    for (unsigned int y = 1; y < heightmap.GetHeight(); ++y)
    {
        Vector2u loc(0, y);

        moreX = outVertices[loc.MoreX()].Pos;
        lessY = outVertices[loc.LessY()].Pos;
        toMoreX = (moreX - outVertices[loc].Pos).Normalized();
        toLessY = (lessY - outVertices[loc].Pos.Normalized());

        outVertices[loc].Normal = toMoreX.Cross(toLessY);
        assert(outVertices[loc].Normal.Dot(-outVertices[loc].Pos) < 0.0f);
    }
}


void WorldGen::GenerateSubdivision(const Array2D<PlanetVertex> & planetVertices, Vector2u min, Vector2u max,
                                   float worldScale, float minHeight, float heightScale,
                                   std::vector<PlanetVertex> & outVertices, std::vector<unsigned int> & outIndices)
{
    Vector2u size = max - min + Vector2u(1, 1);
    assert(size.x > 1 && size.y > 1);
    assert(max.x < planetVertices.GetWidth());
    assert(max.y < planetVertices.GetHeight());

    bool coversTop = (min.y == 0),
         coversBottom = (max.y == planetVertices.GetHeight() - 1);

    outVertices.reserve((size.x * size.y) + (coversTop ? 1 : 0) + (coversBottom ? 1 : 0));
    outIndices.reserve((outVertices.capacity() * 6) +
                           (coversTop ? ((size.x - 1) * 3) : 0) +
                           (coversBottom ? ((size.x - 1) * 3) : 0));

    //Insert the vertices and indices for the vertex map.
    for (Vector2u loc(0, min.y); loc.y <= max.y; ++loc.y)
    {
        for (loc.x = min.x; loc.x <= max.x; ++loc.x)
        {
            //The part of the calculated "outVertices" index for this vertex based on the x coordinate.
            unsigned int xIndexPart = loc.x - min.x;
            //Wrap the X value around the bounds of the planet map.
            loc.x %= planetVertices.GetWidth();

            outVertices.insert(outVertices.end(), planetVertices[loc]);

            if (loc.x < max.x && loc.y < max.y)
            {
                unsigned int index = xIndexPart + (size.x * (loc.y - min.y));

                outIndices.insert(outIndices.end(), index);
                outIndices.insert(outIndices.end(), index + 1);
                outIndices.insert(outIndices.end(), index + 1 + size.x);

                outIndices.insert(outIndices.end(), index);
                outIndices.insert(outIndices.end(), index + 1 + size.x);
                outIndices.insert(outIndices.end(), index + size.x);
            }
        }
    }

    //If the top/bottom of the planet is covered here, generate top/bottom vertex.
    if (coversTop)
    {
        //Average the top's height and normal.
        float avgHeight = 0.0f;
        Vector3f avgNormal;
        for (unsigned int x = min.x; x <= max.x; ++x)
        {
            avgHeight += outVertices[x - min.x].Heightmap;
            avgNormal += outVertices[x - min.x].Normal;
        }
        avgHeight /= (float)size.x;
        avgNormal /= (float)size.x;

        unsigned int index = outVertices.size();
        outVertices.insert(outVertices.end(), PlanetVertex(Vector3f(0.0f, 0.0f, minHeight + (avgHeight * heightScale)) * worldScale,
                                                           avgNormal, avgHeight));

        //Make a triangle from each adjacent pair of vertices on the top border to the top vertex.
        for (unsigned int x = min.x; x < max.x; ++x)
        {
            outIndices.insert(outIndices.end(), x);
            outIndices.insert(outIndices.end(), x + 1);
            outIndices.insert(outIndices.end(), index);
        }
    }
    if (coversBottom)
    {
        unsigned int indexYBase = (size.x * (max.y - min.y));

        //Average the bottom's height and normal.
        float avgHeight = 0.0f;
        Vector3f avgNormal;
        for (unsigned int x = min.x; x <= max.x; ++x)
        {
            avgHeight += outVertices[(x - min.x) + indexYBase].Heightmap;
            avgNormal += outVertices[(x - min.x) + indexYBase].Normal;
        }
        avgHeight /= (float)size.x;
        avgNormal /= (float)size.x;

        unsigned int index = outVertices.size();
        outVertices.insert(outVertices.end(), PlanetVertex(Vector3f(0.0f, 0.0f, -(minHeight + (avgHeight * heightScale))) * worldScale,
                                                           avgNormal, avgHeight));

        //Make a triangle from each adjacent pair of vertices on the bottom border to the bottom vertex.
        for (unsigned int x = min.x; x < max.x; ++x)
        {
            outIndices.insert(outIndices.end(), indexYBase + x);
            outIndices.insert(outIndices.end(), indexYBase + x + 1);
            outIndices.insert(outIndices.end(), index);
        }
    }
}