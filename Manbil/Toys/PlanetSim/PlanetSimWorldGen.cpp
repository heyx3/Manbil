#include "PlanetSimWorldGen.h"

#include "../../Math/Lower Math/Quaternion.h"
#include "../../DebugAssist.h"
#include "../../Math/NoiseGeneration.hpp"
#include <assert.h>



RenderIOAttributes PlanetVertex::GetVertexAttributes(void)
{
    return RenderIOAttributes(RenderIOAttributes::Attribute(3, false, "vIn_Pos"),
                              RenderIOAttributes::Attribute(3, true, "vIn_Normal"),
                              RenderIOAttributes::Attribute(1, false, "vIn_Heightmap"));
}

//Generates vertices from the given noise grid into the given vertex grid.
//Takes in which face (0=X, 1=Y, 2=Z; either positive or negative on that axis) is being generated.
//Then, generates triangles into the given std::vectors.
//Finally, generates vertex/index buffers and puts them into the given mesh.
void HeightmapToMesh(const Array2D<float>& noiseGrid, Array2D<PlanetVertex>& outVertGrid,
                     unsigned int faceAxis, bool isNegative,
                     float minHeight, float maxHeight,
                     std::vector<PlanetVertex>& vertList, std::vector<unsigned int>& indList,
                     Mesh& outMesh)
{
    assert(noiseGrid.GetWidth() == noiseGrid.GetHeight());

    //Get the max/min of the noise values.
    NoiseAnalysis2D::MinMax minMax = NoiseAnalysis2D::GetMinAndMax(noiseGrid);


    //Make sure the out data is the right size.

    unsigned int nVerts = outVertGrid.GetWidth() * outVertGrid.GetHeight(),
                 nInds = 6 * (outVertGrid.GetWidth() - 1) * (outVertGrid.GetHeight() - 1);

    if (vertList.size() != nVerts)
    {
        vertList.resize(nVerts);
    }
    if (indList.size() != nInds)
    {
        indList.resize(nInds);
    }
    if (outVertGrid.GetWidth() != noiseGrid.GetWidth() ||
        outVertGrid.GetHeight() != noiseGrid.GetHeight())
    {
        outVertGrid.Reset(noiseGrid.GetWidth(), noiseGrid.GetHeight());
    }


    //Figure out how to convert a grid spot to a 3D position.
    Vector3f(*getWorldPos)(float halfSide, Vector2u loc) = 0;
    switch (faceAxis)
    {
        case 0:
            if (isNegative)
            {
                getWorldPos = [](float hfS, Vector2u l)
                              {
                                  return Vector3f(-hfS, (float)l.x - hfS, (float)l.y - hfS);
                              };
            }
            else
            {
                getWorldPos = [](float hfS, Vector2u l)
                              {
                                  return Vector3f(hfS, (float)l.x - hfS, (float)l.y - hfS);
                              };
            }
        break;
        
        case 1:
            if (isNegative)
            {
                getWorldPos = [](float hfS, Vector2u l)
                              {
                                  return Vector3f((float)l.x - hfS, -hfS, (float)l.y - hfS);
                              };
            }
            else
            {
                getWorldPos = [](float hfS, Vector2u l)
                              {
                                  return Vector3f((float)l.x - hfS, hfS, (float)l.y - hfS);
                              };
            }
        break;
        
        case 2:
            if (isNegative)
            {
                getWorldPos = [](float hfS, Vector2u l)
                              {
                                  return Vector3f((float)l.x - hfS, (float)l.y - hfS, -hfS);
                              };
            }
            else
            {
                getWorldPos = [](float hfS, Vector2u l)
                              {
                                  return Vector3f((float)l.x - hfS, (float)l.y - hfS, hfS);
                              };
            }
        break;

        default:
            assert(false);
    }

    //Generate the vertex grid based on the noise values.
    float halfSide = (float)noiseGrid.GetWidth() * 0.5f;
    for (Vector2u loc; loc.y < noiseGrid.GetHeight(); ++loc.y)
    {
        for (loc.x = 0; loc.x < noiseGrid.GetWidth(); ++loc.x)
        {
            PlanetVertex& vert = outVertGrid[loc];

            float heightVal = noiseGrid[loc];
            heightVal = Mathf::Remap(minMax.Min, minMax.Max, 0.0f, 1.0f, heightVal);

            vert.Heightmap = Mathf::Supersmooth(Mathf::Supersmooth(heightVal));

            vert.Pos = getWorldPos(halfSide, loc).Normalized();
            vert.Pos *= Mathf::Lerp(minHeight, maxHeight, vert.Heightmap);
        }
    }
    
    //Create the triangles.
    unsigned int currentIndex = 0;
    for (Vector2u loc; loc.y < outVertGrid.GetHeight(); ++loc.y)
    {
        for (loc.x = 0; loc.x < outVertGrid.GetWidth(); ++loc.x)
        {
            unsigned int tl = loc.x + (outVertGrid.GetWidth() * loc.y),
                         tr = tl + 1,
                         bl = tl + outVertGrid.GetWidth(),
                         br = bl + 1;
            vertList[tl] = outVertGrid[loc];

            if (loc.x < outVertGrid.GetWidth() - 1 && loc.y < outVertGrid.GetHeight() - 1)
            {
                indList[currentIndex++] = tl;
                indList[currentIndex++] = bl;
                indList[currentIndex++] = br;

                indList[currentIndex++] = tl;
                indList[currentIndex++] = br;
                indList[currentIndex++] = tr;
            }
        }
    }

    //Generate normals.
    bool(*shouldFlipNormal)(const Vector3f&, const PlanetVertex&, void*) =
        [](const Vector3f& norm, const PlanetVertex& vertx, void* pData)
    {
        return norm.Dot(vertx.Pos) < 0.0f;
    };
    Geometryf::CalculateNormals<PlanetVertex>(vertList.data(), vertList.size(),
                                indList.data(), indList.size(),
                                shouldFlipNormal, (void*)0);

    //Generate buffers.
    outMesh.SubMeshes.push_back(MeshData(false, PT_TRIANGLE_LIST));
    outMesh.SubMeshes[0].SetVertexData(vertList, MeshData::BUF_STATIC,
                                       PlanetVertex::GetVertexAttributes());
    outMesh.SubMeshes[0].SetIndexData(indList, MeshData::BUF_STATIC);
}

WorldData::WorldData(unsigned int verticesPerSide, float minH, float maxH)
    : minHeight(minH), maxHeight(maxH),
      pNegX(verticesPerSide, verticesPerSide), pNegY(verticesPerSide, verticesPerSide),
      pNegZ(verticesPerSide, verticesPerSide), pPosX(verticesPerSide, verticesPerSide),
      pPosY(verticesPerSide, verticesPerSide), pPosZ(verticesPerSide, verticesPerSide)
{
    //Initialize some data.

    DiamondSquareStep variances[] =
    {
        Interval(0.0f, 210.0f),
        Interval(0.0f, 150.0f),
        Interval(0.0f, 137.0f),
        Interval(0.0f, 90.0f),
        Interval(0.0f, 50.0f),
        Interval(0.0f, 35.0f),
        Interval(0.0f, 25.0f),
        Interval(0.0f, 12.0f),
        Interval(0.0f, 5.0f),
    };
    DiamondSquare sqr(131415, Interval(0.0f, 0.0025f), variances, sizeof(variances) / sizeof(DiamondSquareStep), 0.0f);

    Array2D<float> outNoise(verticesPerSide, verticesPerSide);


    //Generate the faces.

    std::vector<PlanetVertex> vertices;
    std::vector<unsigned int> indices;

    //Store the generated edges in order to keep them the same across the terrain faces.
    std::vector<float> edgeNegXY(verticesPerSide), edgeNegXZ(verticesPerSide), edgeNegYZ(verticesPerSide),
                       edgePosXY(verticesPerSide), edgePosXZ(verticesPerSide), edgePosYZ(verticesPerSide),
                       edgeNegXPosY(verticesPerSide), edgeNegXPosZ(verticesPerSide), edgeNegYPosZ(verticesPerSide),
                       edgePosXNegY(verticesPerSide), edgePosXNegZ(verticesPerSide), edgePosYNegZ(verticesPerSide);
    

    //Generate the negative X face heightmap.

    outNoise.Fill(Mathf::NaN);
    sqr.Generate(outNoise);

    //Save the edges.
    for (unsigned int i = 0; i < verticesPerSide; ++i)
    {
        edgeNegXY[i] = outNoise[Vector2u(0, i)];
        edgeNegXZ[i] = outNoise[Vector2u(i, 0)];
        edgeNegXPosY[i] = outNoise[Vector2u(verticesPerSide - 1, i)];
        edgeNegXPosZ[i] = outNoise[Vector2u(i, verticesPerSide - 1)];
    }

    HeightmapToMesh(outNoise, pNegX, 0, true, minHeight, maxHeight, vertices, indices, mNegX);



    //Generate the negative Y face.

    outNoise.Fill(Mathf::NaN);
    for (unsigned int y = 0; y < verticesPerSide; ++y)
        outNoise[Vector2u(0, y)] = edgeNegXY[y];
    sqr.Seed = 268281371;
    sqr.Generate(outNoise);

    //Assert that the edges are the same.
    for (unsigned int y = 0; y < verticesPerSide; ++y)
        assert(outNoise[Vector2u(0, y)] == edgeNegXY[y]);

    //Save the edges.
    for (unsigned int i = 0; i < verticesPerSide; ++i)
    {
        edgeNegYZ[i] = outNoise[Vector2u(i, 0)];
        edgeNegYPosZ[i] = outNoise[Vector2u(i, verticesPerSide - 1)];
        edgePosXNegY[i] = outNoise[Vector2u(verticesPerSide - 1, i)];
    }

    HeightmapToMesh(outNoise, pNegY, 1, true, minHeight, maxHeight, vertices, indices, mNegY);


    //Generate the positive X face.

    outNoise.Fill(Mathf::NaN);
    for (unsigned int y = 0; y < verticesPerSide; ++y)
        outNoise[Vector2u(0, y)] = edgePosXNegY[y];
    sqr.Seed = 2612633;
    sqr.Generate(outNoise);

    //Assert that the edges are the same.
    for (unsigned int y = 0; y < verticesPerSide; ++y)
        assert(outNoise[Vector2u(0, y)] == edgePosXNegY[y]);

    //Save the edges.
    for (unsigned int i = 0; i < verticesPerSide; ++i)
    {
        edgePosXY[i] = outNoise[Vector2u(verticesPerSide - 1, i)];
        edgePosXNegZ[i] = outNoise[Vector2u(i, 0)];
        edgePosXZ[i] = outNoise[Vector2u(i, verticesPerSide - 1)];
    }

    HeightmapToMesh(outNoise, pPosX, 0, false, minHeight, maxHeight, vertices, indices, mPosX);


    //Generate the positive Y face.

    outNoise.Fill(Mathf::NaN);
    for (unsigned int y = 0; y < verticesPerSide; ++y)
    {
        outNoise[Vector2u(0, y)] = edgeNegXPosY[y];
        outNoise[Vector2u(verticesPerSide - 1, y)] = edgePosXY[y];
    }
    sqr.Seed = 919181;
    sqr.Generate(outNoise);

    //Assert that the edges are the same.
    for (unsigned int y = 0; y < verticesPerSide; ++y)
    {
        assert(outNoise[Vector2u(0, y)] == edgeNegXPosY[y]);
        assert(outNoise[Vector2u(verticesPerSide - 1, y)] == edgePosXY[y]);
    }

    //Save the edges.
    for (unsigned int x = 0; x < verticesPerSide; ++x)
    {
        edgePosYNegZ[x] = outNoise[Vector2u(x, 0)];
        edgePosYZ[x] = outNoise[Vector2u(x, verticesPerSide - 1)];
    }

    HeightmapToMesh(outNoise, pPosY, 1, false, minHeight, maxHeight, vertices, indices, mPosY);


    //Generate the negative Z face.

    outNoise.Fill(Mathf::NaN);
    for (unsigned int i = 0; i < verticesPerSide; ++i)
    {
        outNoise[Vector2u(i, 0)] = edgeNegYZ[i];
        outNoise[Vector2u(i, verticesPerSide - 1)] = edgePosYNegZ[i];
        outNoise[Vector2u(0, i)] = edgeNegXZ[i];
        outNoise[Vector2u(verticesPerSide - 1, i)] = edgePosXNegZ[i];
    }
    sqr.Seed = 169823;
    sqr.Generate(outNoise);

    //Assert that the edges are the same.
    for (unsigned int i = 0; i < verticesPerSide; ++i)
    {
        assert(outNoise[Vector2u(i, 0)] == edgeNegYZ[i]);
        assert(outNoise[Vector2u(i, verticesPerSide - 1)] == edgePosYNegZ[i]);
        assert(outNoise[Vector2u(0, i)] == edgeNegXZ[i]);
        assert(outNoise[Vector2u(verticesPerSide - 1, i)] == edgePosXNegZ[i]);
    }

    HeightmapToMesh(outNoise, pNegZ, 2, true, minHeight, maxHeight, vertices, indices, mNegZ);

    
    //Generate the positive Z face.

    outNoise.Fill(Mathf::NaN);
    for (unsigned int i = 0; i < verticesPerSide; ++i)
    {
        outNoise[Vector2u(i, 0)] = edgeNegYPosZ[i];
        outNoise[Vector2u(i, verticesPerSide - 1)] = edgePosYZ[i];
        outNoise[Vector2u(0, i)] = edgeNegXPosZ[i];
        outNoise[Vector2u(verticesPerSide - 1, i)] = edgePosXZ[i];
    }
    sqr.Seed = 54846;
    sqr.Generate(outNoise);

    //Assert that the edges are the same.
    for (unsigned int i = 0; i < verticesPerSide; ++i)
    {
        assert(outNoise[Vector2u(i, 0)] == edgeNegYPosZ[i]);
        assert(outNoise[Vector2u(i, verticesPerSide - 1)] == edgePosYZ[i]);
        assert(outNoise[Vector2u(0, i)] == edgeNegXPosZ[i]);
        assert(outNoise[Vector2u(verticesPerSide - 1, i)] == edgePosXZ[i]);
    }

    HeightmapToMesh(outNoise, pPosZ, 2, false, minHeight, maxHeight, vertices, indices, mPosZ);
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