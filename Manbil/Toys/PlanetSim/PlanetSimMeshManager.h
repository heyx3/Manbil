#pragma once

#include "../../Mesh.h"


//Manages the planet's terrain meshes.
class PlanetSimMeshManager
{
public:

    PlanetSimMeshManager(void) : planetMeshes(1, 1, Mesh(PrimitiveTypes::TriangleList)) { }

    //The minimum-possible height of a terrain vertex.
    float GetMinHeight(void) const { return minHeight; }
    //The scale of the terrain height.
    //Assuming a terrain's heightmap is between 0 and 1, the range of terrain height is [minHeight, minHeight + heightScale].
    float GetHeightScale(void) const { return heightScale; }

    //Generates the planet's meshes.
    void GeneratePlanet(const Array2D<float> & heightmap, float planetScale,
                        float minHeight, float heightScale, Vector2u subdivSize);
    //Gets all the pieces of this planet's terrain that are visible.
    void GetVisibleTerrain(Vector3f camPos, std::vector<const Mesh*> & outMeshes);

    template<typename Func>
    //Does the given function to every mesh. The function should have the signature "void Do(Vector2u subdivIndex, Mesh & mesh)".
    void DoToEveryMesh(Func func)
    {
        for (Vector2u subdivIndex; subdivIndex.y < planetMeshes.GetHeight(); ++subdivIndex.y)
            for (subdivIndex.x = 0; subdivIndex.x < planetMeshes.GetWidth(); ++subdivIndex.x)
                func(subdivIndex, planetMeshes[subdivIndex]);
    }

private:

    Array2D<Mesh> planetMeshes;
    float minHeight, heightScale;
};