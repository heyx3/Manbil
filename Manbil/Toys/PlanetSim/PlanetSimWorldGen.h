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



//Generates and manages the world vertices.
class WorldData
{
public:


    float GetMinHeight(void) const { return minHeight; }
    float GetMaxHeight(void) const { return maxHeight; }


    WorldData(unsigned int verticesPerSide, float minHeight, float maxHeight);

    void GetMeshes(std::vector<const Mesh*> & outMeshes, Vector3f camPos, Vector3f camForward) const;
    float GetHeightAt(Vector3f worldPos) const;


private:

    Mesh mNegX, mNegY, mNegZ, mPosX, mPosY, mPosZ;
    Array2D<PlanetVertex> pNegX, pNegY, pNegZ, pPosX, pPosY, pPosZ;
    float minHeight, maxHeight;
};