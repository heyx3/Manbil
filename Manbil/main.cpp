#include "SFML/Graphics.hpp"

#include "Math/Lower Math/Array3D.h"

#include "IO/XmlSerialization.h"
#include "IO/BinarySerialization.h"

#include "Sample Worlds/SimpleRenderWorld.h"
#include "Sample Worlds/DataNodeRenderWorld.h"
#include "Sample Worlds/WaterWorld.h"

#include "Sample Worlds/GUIWorld.h"

#include "Sample Worlds/OpenGLTestWorld.h"
#include "Sample Worlds/SerializationWorld.h"
#include "Tests/RiftTestWorld.h"
#include "Tests/AssImpTestWorld.h"
#include "Tests/TerrainTestWorld.h"
#include "Tests/GUITestWorld.h"
#include "Sample Worlds/NoiseTest.h"
#include "Toys/TwoTrianglesWorld.h"
#include "Toys/Voxel/VoxelWorld.h"
#include "Toys/PlanetSim/PlanetSimWorld.h"


//TODO: Add a "Skybox" class in "Rendering/Helper Classes" that simplifies creation/modification/rendering of a cubemapped skybox.


int main()
{
    SimpleRenderWorld().RunWorld();
    //DataNodeRenderWorld().RunWorld();
    //WaterWorld().RunWorld();
    //SerializationWorld().RunWorld();

    //GUIWorld().RunWorld();
    //OpenGLTestWorld().RunWorld();
    //AssImpTestWorld().RunWorld();
    //TerrainTestWorld().RunWorld();
    //GUITestWorld().RunWorld();
    

    //NoiseTest().RunWorld();


    //TwoTrianglesWorld().RunWorld();
    //VoxelWorld().RunWorld();
    //PlanetSimWorld().RunWorld();
}