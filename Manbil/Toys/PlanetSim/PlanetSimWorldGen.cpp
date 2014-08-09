#include "PlanetSimWorldGen.h"

#include "../../Math/Quaternion.h"
#include "../../DebugAssist.h"
#include "../../Math/NoiseGeneration.hpp"
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


WorldData::WorldData(unsigned int verticesPerSide, float minH, float maxH,
                     unsigned int size, float scale, unsigned int levels)
    : minHeight(minH), maxHeight(maxH),
      mNegX(TriangleList), mPosX(TriangleList), mNegY(TriangleList),
      mPosY(TriangleList), mNegZ(TriangleList), mPosZ(TriangleList),
      pNegX(verticesPerSide, verticesPerSide), pNegY(verticesPerSide, verticesPerSide),
      pNegZ(verticesPerSide, verticesPerSide), pPosX(verticesPerSide, verticesPerSide),
      pPosY(verticesPerSide, verticesPerSide), pPosZ(verticesPerSide, verticesPerSide)
{
    //Create the perlin noise layers.
    std::vector<Perlin3D> pers;
    std::vector<float> weights;
    for (unsigned int level = 0; level < levels; ++level)
    {
        float weight = powf(0.5f, (float)(level + 1));
        unsigned int repeatInterval = BasicMath::RoundToInt((float)size * scale * weight);

        pers.insert(pers.end(), Perlin3D(scale * powf(0.5f, (float)level),
                                         Perlin3D::Quintic, Vector3i(),
                                         Vector3i(162613, level, level + 1362361).GetHashCode(),
                                         true, Vector3u(repeatInterval, repeatInterval, repeatInterval)));
        weights.insert(weights.end(), weight);
    }

    //Combine the perlin noise layers.
    std::vector<const Generator3D*> pPers;
    for (unsigned int i = 0; i < pers.size(); ++i)
        pPers.insert(pPers.end(), &pers[i]);
    LayeredOctave3D fractalNoise(pers.size(), weights.data(), pPers.data());
    
    Array3D<float> surfaceNoise(size, size, size);
    fractalNoise.Generate(surfaceNoise);

    float midHeight = (minHeight + maxHeight) * 0.5f;
    unsigned int halfSide = verticesPerSide / 2;

    //Generate the vertices for each face, then the triangles, then the vertex normals.
    for (unsigned int i = 0; i < 6; ++i)
    {
        //Collect data that varies by face.
        Array2D<PlanetVertex> * vs = 0;
        Mesh * msh = 0;
        Vector3i(*locToPos)(Vector2u loc, unsigned int halfSide);

        switch (i)
        {
            case 0:
                vs = &pNegX;
                msh = &mNegX;
                locToPos = [](Vector2u loc, unsigned int halfSide)
                    { return Vector3i(-halfSide, loc.x - halfSide, loc.y - halfSide); };
                break;

            case 1:
                vs = &pPosX;
                msh = &mPosX;
                locToPos = [](Vector2u loc, unsigned int halfSide)
                    { return Vector3i(halfSide, loc.x - halfSide, loc.y - halfSide); };
                break;

            case 2:
                vs = &pNegY;
                msh = &mNegY;
                locToPos = [](Vector2u loc, unsigned int halfSide)
                    { return Vector3i(loc.x - halfSide, -halfSide, loc.y - halfSide); };
                break;

            case 3:
                vs = &pPosY;
                msh = &mPosY;
                locToPos = [](Vector2u loc, unsigned int halfSide)
                    { return Vector3i(loc.x - halfSide, halfSide, loc.y - halfSide); };
                break;

            case 4:
                vs = &pNegZ;
                msh = &mNegZ;
                locToPos = [](Vector2u loc, unsigned int halfSide)
                    { return Vector3i(loc.x - halfSide, loc.y - halfSide, -halfSide); };
                break;

            case 5:
                vs = &pPosZ;
                msh = &mPosZ;
                locToPos = [](Vector2u loc, unsigned int halfSide)
                    { return Vector3i(loc.x - halfSide, loc.y - halfSide, halfSide); };
                break;

            default: assert(false);
        }

        //Convert noise values to vertices for this face.
        for (Vector2u loc; loc.y < verticesPerSide; ++loc.y)
        {
            for (loc.x = 0; loc.x < verticesPerSide; ++loc.x)
            {
                PlanetVertex * vert = &vs->operator[](loc);

                Vector3i posI = locToPos(loc, halfSide);
                vert->Pos = ToV3f(posI).Normalized() * midHeight;

                posI = vert->Pos.RoundToInt();
                vert->Heightmap = surfaceNoise[surfaceNoise.Wrap(posI).CastToUInt()];
                vert->Heightmap = BasicMath::Supersmooth(vert->Heightmap);
                vert->Heightmap = BasicMath::Supersmooth(vert->Heightmap);

                vert->Pos = (vert->Pos / midHeight) *
                            BasicMath::Lerp(minHeight, maxHeight, vert->Heightmap);
            }
        }

        //Calculate vertices/indices.
        std::vector<PlanetVertex> vertices;
        vertices.resize(vs->GetWidth() * vs->GetHeight());
        std::vector<unsigned int> indices;
        indices.resize(6 * (vs->GetWidth() - 1) * (vs->GetHeight() - 1));
        unsigned int currentIndex = 0;
        for (Vector2u loc; loc.y < verticesPerSide; ++loc.y)
        {
            for (loc.x = 0; loc.x < verticesPerSide; ++loc.x)
            {
                unsigned int tl = loc.x + (vs->GetWidth() * loc.y),
                             tr = tl + 1,
                             bl = tl + vs->GetWidth(),
                             br = bl + 1;
                vertices[tl] = vs->operator[](loc);

                if (loc.x < verticesPerSide - 1 && loc.y < verticesPerSide - 1)
                {
                    indices[currentIndex++] = tl;
                    indices[currentIndex++] = bl;
                    indices[currentIndex++] = br;

                    indices[currentIndex++] = tl;
                    indices[currentIndex++] = br;
                    indices[currentIndex++] = tr;
                }
            }
        }

        //Calculate normals.
        for (unsigned int tri = 0; tri < indices.size(); tri += 3)
        {
            Vector3f v_1_2 = vertices[indices[tri]].Pos - vertices[indices[tri + 1]].Pos,
                     v_1_3 = vertices[indices[tri]].Pos - vertices[indices[tri + 2]].Pos;
            Vector3f norm = v_1_2.Normalized().Cross(v_1_3.Normalized());

            if (norm.Dot(vertices[indices[tri]].Pos) > 0.0f)
                norm = -norm;

            vertices[indices[tri]].Normal += norm;
            vertices[indices[tri + 1]].Normal += norm;
            vertices[indices[tri + 2]].Normal += norm;
        }
        for (unsigned int i = 0; i < vertices.size(); ++i)
            vertices[i].Normal.Normalize();

        //Generate the mesh.
        RenderObjHandle vbo, ibo;
        RenderDataHandler::CreateVertexBuffer(vbo, vertices.data(), vertices.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
        RenderDataHandler::CreateIndexBuffer(ibo, indices.data(), indices.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
        msh->SetVertexIndexData(VertexIndexData(vertices.size(), vbo, indices.size(), ibo));
    }
}

void WorldData::GetMeshes(std::vector<const Mesh*> & outMeshes, Vector3f camPos, Vector3f camForward) const
{
    outMeshes.insert(outMeshes.end(), &mNegX);
    outMeshes.insert(outMeshes.end(), &mNegY);
    outMeshes.insert(outMeshes.end(), &mNegZ);
    outMeshes.insert(outMeshes.end(), &mPosX);
    outMeshes.insert(outMeshes.end(), &mPosY);
    outMeshes.insert(outMeshes.end(), &mPosZ);
}