#include "VoxelWorld.h"

#include <iostream>


namespace VWErrors
{
    void PauseConsole(void)
    {
        std::cout << "Enter a key to continue: ";
        char dummy;
        std::cin >> dummy;
        std::cout << "\n\n";
    }
    void WriteError(std::string intro, std::string error)
    {
        std::cout << intro << ": " << error << "\n\n";
    }
}
using namespace VWErrors;



Vector2i windowSize(400, 400);


VoxelWorld::VoxelWorld(void)
    : SFMLOpenGLWorld(windowSize.x, windowSize.y, sf::ContextSettings(8, 0, 0, 3, 1)),
        chunk(Vector3i(0, 0, 0)), voxelMat(0), renderState(),
        cam(5.0f)
{

}

void VoxelWorld::InitializeWorld(void)
{

}
void VoxelWorld::OnWorldEnd(void)
{
    if (voxelMat != 0) delete voxelMat;
}

void OnWindowResized(unsigned int w, unsigned int h)
{
    glViewport(0, 0, w, h);
    windowSize.x = w;
    windowSize.y = h;
}

void VoxelWorld::UpdateWorld(float elapsed)
{
    if (cam.Update(elapsed))
        EndWorld();
}

void VoxelWorld::RenderOpenGL(float elapsed)
{

}