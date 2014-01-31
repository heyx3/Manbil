#include "SFML/Graphics.hpp"
#include "NoiseTest.h"
#include "TerrainWorld.h"
#include "OpenGLTestWorld.h"
#include "TwoTrianglesWorld.h"

#include "IO/IOTest.h"
using namespace tinyxml2;

#include <iostream>
using namespace std;


void PauseConsole(void)
{
	char dummy;
	cin >> dummy;
}

void RunIOTest(void)
{
	IOTester tester;
	
	XMLDocument doc;
	XMLError error = doc.LoadFile("TestIOData.xml");
	if (error != 0)
	{
		cout << "Error reading xml document: " << error << "\n";
		PauseConsole();
		return;
	}
}


int main()
{
	//PrintMatrix(GetMatrixToPrint());
	//PauseConsole();

	//NoiseTest().RunWorld();
	//TerrainWorld().RunWorld();
	//OpenGLTestWorld().RunWorld();

	//TwoTrianglesWorld().RunWorld();

	RunIOTest();
}