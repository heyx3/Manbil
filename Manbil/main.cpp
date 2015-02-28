#include "SFML/Graphics.hpp"

#include "Math/Lower Math/Array3D.h"

#include "IO/XmlSerialization.h"
#include "IO/BinarySerialization.h"

#include "OpenGLTestWorld.h"
#include "Tests/RiftTestWorld.h"
#include "Tests/AssImpTestWorld.h"
#include "Tests/TerrainTestWorld.h"
#include "Tests/GUITestWorld.h"
#include "NoiseTest.h"
#include "TwoTrianglesWorld.h"
#include "Toys/Voxel/VoxelWorld.h"
#include "Toys/PlanetSim/PlanetSimWorld.h"


//TODO: Add a "Skybox" class in "Rendering/Helper Classes" that simplifies creation/modification/rendering of a cubemapped skybox.


int main()
{
    //OpenGLTestWorld().RunWorld();
    //RiftTestWorld().RunWorld();
    //AssImpTestWorld().RunWorld();
    //TerrainTestWorld().RunWorld();
    //GUITestWorld().RunWorld();
    

    //NoiseTest().RunWorld();


    //TwoTrianglesWorld().RunWorld();
    VoxelWorld().RunWorld();
    //PlanetSimWorld().RunWorld();
}