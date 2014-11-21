#include "SFML/Graphics.hpp"

#include "Math/Lower Math/Array3D.h"

#include "IO/XmlSerialization.h"
#include "IO/BinarySerialization.h"

#include "NoiseTest.h"
#include "OpenGLTestWorld.h"
#include "TwoTrianglesWorld.h"
#include "Toys/Voxel/VoxelWorld.h"
#include "Tests/TwoDOpenGLTest.h"
#include "Tests/GUITestWorld.h"
#include "Toys/PlanetSim/PlanetSimWorld.h"


#include <iostream>
using namespace std;
void PauseConsole(void)
{
    char dummy;
    cout << "Enter any character to continue.\n";
    cin >> dummy;
}
void WriteToConsole(const std::string & outStr)
{
    cout << outStr << "\n";
}



//PRIORITY: Replace "intentionally not implemented" functions with the ' = delete' syntax.

int main()
{
    //OpenGLTestWorld().RunWorld();
    
    //TwoDOpenGLTest().RunWorld();
    //GUITestWorld().RunWorld();
    
    //NoiseTest().RunWorld();

    //TwoTrianglesWorld().RunWorld();

    //VoxelWorld().RunWorld();
    PlanetSimWorld().RunWorld();
}