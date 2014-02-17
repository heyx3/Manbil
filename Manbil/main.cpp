#include "SFML/Graphics.hpp"

//#include "NoiseTest.h"
//#include "OpenGLTestWorld.h"
//#include "TwoTrianglesWorld.h"
//#include "Tests/TwoDOpenGLTest.h"


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




#include "Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "Rendering/Materials/Data Nodes/ShaderGenerator.h"
void TestDataNodes(void)
{
    #pragma region Set up channels

    std::unordered_map<RenderingChannels, DataLine> channels;

    channels[RenderingChannels::RC_Diffuse] = DataLine(DataNodePtr(new AddNode(DataLine(Vector(Vector3f(10.0f, -32.4f, 24.0f))),
                                                                               DataLine(Vector(Vector3f(-10.0f, 32.4f, -24.0f))))),
                                                       0);

    #pragma endregion


    UniformDictionary uniforms;

    std::string vert, frag;

    LightSettings lightSettings(false);
    ShaderGenerator::GenerateShaders(vert, frag, uniforms, RenderingModes::RM_Opaque, false, lightSettings, channels);

    WriteToConsole("Vertex shader:\n----------------------------------------------\n\n");
    WriteToConsole(vert);
    WriteToConsole("------------------------------------\n\nFragment shader:\n----------------------------------------\n\n");
    WriteToConsole(frag);

    PauseConsole();
}







int main()
{
    TestDataNodes();

    //NoiseTest().RunWorld();
    //OpenGLTestWorld().RunWorld();
    //TwoDOpenGLTest().RunWorld();
    //TwoTrianglesWorld().RunWorld();
}