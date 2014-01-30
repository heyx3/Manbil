#include "SFML/Graphics.hpp"
#include "NoiseTest.h"
#include "TerrainWorld.h"
#include "OpenGLTestWorld.h"
#include "TwoTrianglesWorld.h"

#include <iostream>


using namespace std;


void PauseConsole(void)
{
	char dummy;
	cin >> dummy;
}

void PrintMatrix(Matrix4f toPrint)
{
	const char * const space = "       ";
	for (int x = 0; x < 4; ++x)
	{
		cout << "[" << space;

		for (int y = 0; y < 4; ++y)
		{
			cout << toPrint[Vector2i(x, y)] << space;
		}

		cout << "]" << "\n";
	}
}

Matrix4f GetMatrixToPrint(void)
{
	Vector3f camPos(200.0f, 200.0f, 4.0f);
	Camera cam(camPos, -camPos.Normalized());
	cam.Info = ProjectionInfo(3.14156f * 0.75f, 4.0f, 3.0f, 1.0f, 1000.0f);
	
	Matrix4f world, view, proj;
	world.SetAsIdentity();
	cam.GetViewTransform(view);
	proj.SetAsPerspProj(cam.Info);

	Matrix4f wvp;
	wvp.SetAsWVP(proj, view, world);

	return wvp.Inverse();
}

int main()
{
	//PrintMatrix(GetMatrixToPrint());
	//PauseConsole();

	//NoiseTest().RunWorld();
	//TerrainWorld().RunWorld();
	OpenGLTestWorld().RunWorld();

	//TwoTrianglesWorld().RunWorld();
}