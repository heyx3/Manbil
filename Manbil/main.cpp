#include "SFML/Graphics.hpp"

#include "Math/Lower Math/Array3D.h"

#include "IO/XmlSerialization.h"
#include "IO/BinarySerialization.h"

#include "OpenGLTestWorld.h"
#include "Tests/RiftTestWorld.h"
#include "Tests/AssImpTestWorld.h"
#include "Tests/GUITestWorld.h"
#include "NoiseTest.h"
#include "TwoTrianglesWorld.h"
#include "Toys/Voxel/VoxelWorld.h"
#include "Toys/PlanetSim/PlanetSimWorld.h"


#include <iostream>
using namespace std;
void PauseConsole(void)
{
    char dummy;
    cout << "Enter any character to continue.\n";
    cin >> dummy;
}
void WriteToConsole(const std::string& outStr)
{
    cout << outStr << "\n";
}



//TODO: Replace "intentionally not implemented" functions with the ' = delete' syntax.
//TODO: Move static declarations in world CPP files to member fields to eliminate large static allocation.
//TODO: Add a "Skybox" class in "Rendering/Helper Classes" that simplifies creation/modification/rendering of a cubemapped skybox.


#include "DebugAssist.h"
int main()
{
    OpenGLTestWorld().RunWorld();
    //RiftTestWorld().RunWorld();
    //AssImpTestWorld().RunWorld();
    //GUITestWorld().RunWorld();
    

    //NoiseTest().RunWorld();


    //TwoTrianglesWorld().RunWorld();
    //VoxelWorld().RunWorld();
    //PlanetSimWorld().RunWorld();
}