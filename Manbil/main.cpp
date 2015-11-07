#include "Sample Worlds/SimpleRenderWorld.h"
#include "Sample Worlds/DataNodeRenderWorld.h"
#include "Sample Worlds/WaterWorld.h"
#include "Sample Worlds/TerrainWorld.h"
#include "Sample Worlds/SerializationWorld.h"
#include "Sample Worlds/GUIWorld.h"
#include "Sample Worlds/EditorGUIWorld.h"
#include "Sample Worlds/AssetImporterWorld.h"

#include "Sample Worlds/NoiseTest.h"
#include "Toys/TwoTrianglesWorld.h"
#include "Toys/Voxel/VoxelWorld.h"
#include "Toys/PlanetSim/PlanetSimWorld.h"


//TODO: Add a "Skybox" class in "Rendering/Helper Classes" that simplifies creation/modification/rendering of a cubemapped skybox.


int main()
{
    SimpleRenderWorld().RunWorld();
    DataNodeRenderWorld().RunWorld();
    WaterWorld().RunWorld();
    TerrainWorld().RunWorld();
    SerializationWorld().RunWorld();
    GUIWorld().RunWorld();
    EditorGUIWorld().RunWorld();
    AssetImporterWorld().RunWorld();
    

    //NoiseTest().RunWorld();


    //TwoTrianglesWorld().RunWorld();
    //VoxelWorld().RunWorld();
    //PlanetSimWorld().RunWorld();

    return 0;
}