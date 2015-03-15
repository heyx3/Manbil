#include "OldOnesWorld.h"

#include <iostream>
#include "../IO/XmlSerialization.h"


OldOnesWorld::OldOnesWorld(void)
    : windowSize(800, 600),
      SFMLOpenGLWorld(800, 600, sf::ContextSettings())
{
}

sf::VideoMode OldOnesWorld::GetModeToUse(unsigned int windowW, unsigned int windowH)
{
    //Change this return value to change the window resolution mode.
    //To use native fullscreen, return "sf::VideoMode::getFullscreenModes()[0];".
    return sf::VideoMode(windowW, windowH);
}
std::string OldOnesWorld::GetWindowTitle(void)
{
    //Change this to change the string on the window's title-bar
    //    (assuming it has a title-bar).
    return "World window";
}
sf::Uint32 OldOnesWorld::GetSFStyleFlags(void)
{
    //Change this to change the properties of the window.
    return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
}

void OldOnesWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();
    //If there was an error initializing the game, don’t bother with
    //    the rest of initialization.
    if (IsGameOver())
    {
        return;
    }


    //Load world objects.
    GeoSets sets;
    XmlReader reader("Content/Old Ones/WorldGeoObjects.xml");
    reader.ReadDataStructure(sets);
    std::string err;
    for (unsigned int i = 0; i < sets.Sets.size(); ++i)
    {
        objs.push_back(std::shared_ptr<WorldObject>(new WorldObject(sets.Sets[i], err)));
        if (!err.empty())
        {
            std::cout << "Error creating world object '" <<
                         sets.Sets[i].MeshFile << "': " << err << "\n";

            char dummy;
            std::cin >> dummy;

            EndWorld();
            return;
        }
    }


    //Set up camera.
    gameCam = MovingCamera(Vector3f(150.278f, 3.134f, 7.772f),
                           20.0f, 1.0f,
                           Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
    gameCam.PerspectiveInfo.SetFOVDegrees(60.0f);
    gameCam.PerspectiveInfo.Width = windowSize.x;
    gameCam.PerspectiveInfo.Height = windowSize.y;
    gameCam.PerspectiveInfo.zNear = 0.1f;
    gameCam.PerspectiveInfo.zFar = 1000.0f;
    gameCam.Window = GetWindow();
}
void OldOnesWorld::OnWorldEnd(void)
{
    objs.clear();
}

void OldOnesWorld::UpdateWorld(float elapsedSeconds)
{
    gameCam.Update(elapsedSeconds);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
        return;
    }
}
void OldOnesWorld::RenderOpenGL(float elapsedSeconds)
{
    //Set up rendering state.
    //Modify these constructors to change various aspects of how rendering is done.
    ScreenClearer(true, true, false, Vector4f(0.4f, 0.4f, 0.4f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE).EnableState();
    BlendMode::GetOpaque().EnableMode();

    glViewport(0, 0, windowSize.x, windowSize.y);

    Matrix4f viewM, projM;
    gameCam.GetViewTransform(viewM);
    gameCam.GetPerspectiveProjection(projM);
    RenderInfo info(GetTotalElapsedSeconds(), &gameCam, &viewM, &projM);

    for (unsigned int i = 0; i < objs.size(); ++i)
    {
        objs[i]->Render(info);
    }
}

void OldOnesWorld::OnInitializeError(std::string errorMsg)
{
    //Print the error message and end the game.
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    EndWorld();
}
void OldOnesWorld::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
    windowSize.x = newWidth;
    windowSize.y = newHeight;

    gameCam.PerspectiveInfo.Width = newWidth;
    gameCam.PerspectiveInfo.Height = newHeight;
}