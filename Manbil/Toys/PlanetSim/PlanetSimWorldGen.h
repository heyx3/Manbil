#pragma once

#include "../../Math/Array2D.h"
#include "../../Math/Array3D.h"
#include "../../Vertices.h"
#include "../../Mesh.h"
#include <vector>


//Divide the planet's surface into "subdivisions" (rectangular regions on the surface).
//Each subdivision is rendered as a unit and has its own LOD.

//Has a position, a normal, and a "height" from 0-1 from the original heightmap.
struct PlanetVertex
{
    Vector3f Pos, Normal;
    float Heightmap;
    PlanetVertex(Vector3f pos = Vector3f(), Vector3f normal = Vector3f(0.0f, 0.0f, 1.0f), float heightmap = 0.5f)
        : Pos(pos), Normal(normal), Heightmap(heightmap)
    {

    }

    static ShaderInOutAttributes GetAttributeData(void) { return ShaderInOutAttributes(3, 3, 1, false, true, false, "vIn_Pos", "vIn_Normal", "vIn_Heightmap"); }
};


//Handles world vertex generation.
class WorldGen
{
public:

    //Generates the vertices/indices for the whole planet.
    //Automatically resizes the out array to be the right size if it isn't already.
    static void GenerateVertices(const Array2D<float> & heightmap, Array2D<PlanetVertex> & outVertices,
                                 float worldScale, float minHeight, float heightScale);

    //Generates the vertices/indices for a single region of space on the surface of the given planet.
    //The "min" and "max" values are inclusive and wrap horizontally around the vertex map.
    static void GenerateSubdivision(const Array2D<PlanetVertex> & planetVertices, Vector2u min, Vector2u max,
                                    float worldScale, float minHeight, float heightScale,
                                    std::vector<PlanetVertex> & outVertices, std::vector<unsigned int> & outIndices);
};



//Generates and manages the world vertices.
class WorldData
{
public:


    WorldData(unsigned int verticesPerSide, float minHeight, float maxHeight,
              unsigned int noiseSize, float noiseScale, unsigned int noiseLevels);

    void GetMeshes(std::vector<const Mesh*> & outMeshes, Vector3f camPos, Vector3f camForward) const;
    float GetHeightAt(Vector3f worldPos) const;


private:

    Mesh mNegX, mNegY, mNegZ, mPosX, mPosY, mPosZ;
    Array2D<PlanetVertex> pNegX, pNegY, pNegZ, pPosX, pPosY, pPosZ;
    float minHeight, maxHeight;
};