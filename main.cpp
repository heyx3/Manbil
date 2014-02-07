#include "SFML/Graphics.hpp"
#include "NoiseTest.h"
#include "OpenGLTestWorld.h"
#include "TwoTrianglesWorld.h"
#include "Tests/TwoDOpenGLTest.h"

#include <iostream>
using namespace std;


void PauseConsole(void)
{
    char dummy;
    cout << "Enter any character to continue.\n";
    cin >> dummy;
}


int main()
{
    //NoiseTest().RunWorld();

    OpenGLTestWorld().RunWorld();
    //TwoDOpenGLTest().RunWorld();

    //TwoTrianglesWorld().RunWorld();
}