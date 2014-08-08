#include "PlanetSimMeshManager.h"

#include "PlanetSimWorldGen.h"


void PlanetSimMeshManager::GeneratePlanet(const Array2D<float> & heightmap, float planetScale,
                                          float minHeight, float heightScale, Vector2u subdivSize)
{
    assert(BasicMath::IsMultiple(heightmap.GetWidth(), subdivSize.x));
    assert(BasicMath::IsMultiple(heightmap.GetHeight(), subdivSize.y));


    Array2D<PlanetVertex> planetVertices(heightmap.GetWidth(), heightmap.GetHeight());
    WorldGen::GenerateVertices(heightmap, planetVertices, planetScale, minHeight, heightScale);

    std::vector<PlanetVertex> subdivVerts;
    std::vector<unsigned int> subdivInds;

    Vector2u numbSubdivs(heightmap.GetWidth() / subdivSize.x, heightmap.GetHeight() / subdivSize.y);
    planetMeshes.Reset(numbSubdivs.x, numbSubdivs.y);

    for (Vector2u subdiv; subdiv.y < numbSubdivs.y; ++subdiv.y)
    {
        for (subdiv.x = 0; subdiv.x < numbSubdivs.x; ++subdiv.x)
        {
            subdivVerts.clear();
            subdivInds.clear();

            Vector2u start = subdiv.ComponentProduct(subdivSize);
            WorldGen::GenerateSubdivision(planetVertices, start, heightmap.Clamp(start + subdivSize), planetScale, minHeight, heightScale, subdivVerts, subdivInds);

            RenderObjHandle vbo, ibo;
            RenderDataHandler::CreateVertexBuffer(vbo, subdivVerts.data(), subdivVerts.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
            RenderDataHandler::CreateIndexBuffer(ibo, subdivInds.data(), subdivInds.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);

            planetMeshes[subdiv] = Mesh(PrimitiveTypes::TriangleList);
            planetMeshes[subdiv].SetVertexIndexData(VertexIndexData(subdivVerts.size(), vbo, subdivInds.size(), ibo));
        }
    }
}

void PlanetSimMeshManager::GetVisibleTerrain(Vector3f camPos, std::vector<const Mesh*> & outMeshes)
{
    outMeshes.reserve(planetMeshes.GetArea());
    for (Vector2u subdiv; subdiv.y < planetMeshes.GetHeight(); ++subdiv.y)
        for (subdiv.x = 0; subdiv.x < planetMeshes.GetWidth(); ++subdiv.x)
            outMeshes.insert(outMeshes.end(), &planetMeshes[subdiv]);
}