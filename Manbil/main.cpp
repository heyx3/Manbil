#include "SFML/Graphics.hpp"

#include "Math/Array3D.h"

#include "NoiseTest.h"
#include "OpenGLTestWorld.h"
#include "TwoTrianglesWorld.h"
#include "Voxel/VoxelWorld.h"
#include "Tests/TwoDOpenGLTest.h"
#include "Tests/GUITestWorld.h"


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


template<class Vector>
string ToString(Vector v)
{
    string str = "";
    int size = sizeof(Vector) / sizeof(float);

    for (int axis = 0; axis < size; ++axis)
    {
        str += std::to_string(v[axis]);
        if (axis < size - 1)
            str += ", ";
    }
    
    return str;
}


//PRIORITY: Apparently you can declare a function header like "void Func(void) = delete" to define it but prevent calls to it. Use this for things like copy constructors that are currently declared but not implemented.

int main()
{
    //OpenGLTestWorld().RunWorld();
    
    //TwoDOpenGLTest().RunWorld();
    //GUITestWorld().RunWorld();

    //NoiseTest().RunWorld();

    //TwoTrianglesWorld().RunWorld();

    VoxelWorld().RunWorld();
}