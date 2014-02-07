#include "TwoDOpenGLTest.h"

#include "../ScreenClearer.h"
#include "../TextureSettings.h"
#include "../Input/Input Objects/KeyboardBoolInput.h"


#include <iostream>
namespace TwoDOpenGLTestStuff
{
    Vector2i windowSize(200, 200);

    void Pause()
    {
        char dummy;
        std::cin >> dummy;
    }
}
using namespace TwoDOpenGLTestStuff;



TwoDOpenGLTest::TwoDOpenGLTest(void)
    : SFMLOpenGLWorld(windowSize.x, windowSize.y,
                      sf::ContextSettings(24, 0, 1, 3, 3)),
      cam(0), foreQuad(0), backQuad(0), backMat(0), foreMat(0)
{

}

void TwoDOpenGLTest::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    GetWindow()->setVerticalSyncEnabled(true);
    GetWindow()->setMouseCursorVisible(true);

    glViewport(0, 0, windowSize.x, windowSize.y);

    Input.AddBoolInput(1, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::W, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(0, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::S, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(2, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Down, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(3, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Up, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(1234, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Escape, BoolInput::ValueStates::IsDown)));


    RenderingState(true, true, true).EnableState();

    sf::Image background, foreground;
    if (!background.loadFromFile("Water.png"))
    {
        std::cout << "Error loading 'Water.png'\n";
        Pause();
        EndWorld();
        return;
    }
    if (!foreground.loadFromFile("shrub.png"))
    {
        std::cout << "Error loading 'shrub.png'\n";
        Pause();
        EndWorld();
        return;
    }

    TextureSettings setts(TextureSettings::TF_LINEAR, TextureSettings::TW_CLAMP, true);
    RenderObjHandle foreTex, backTex;

    RenderDataHandler::CreateTexture2D(foreTex, foreground);
    setts.SetData(foreTex);
    RenderDataHandler::CreateTexture2D(backTex, background);
    setts.SetData(backTex);

    std::string err = GetCurrentRenderingError();
    if (!err.empty())
    {
        std::cout << "Error creating textures: " << err.c_str();
        Pause();
        EndWorld();
        return;
    }

    backMat = new Material(Materials::UnlitTexture);
    backMat->AddUniform("brightness");
    if (backMat->HasError())
    {
        std::cout << "Error creating background mat: " << backMat->GetErrorMessage();
        Pause();
        EndWorld();
        return;
    }
    backMat->SetTexture(0, backTex);
    foreMat = new Material(Materials::UnlitTexture);
    foreMat->AddUniform("brightness");
    if (foreMat->HasError())
    {
        std::cout << "Error creating foreground mat: " << foreMat->GetErrorMessage();
        Pause();
        EndWorld();
        return;
    }
    foreMat->SetTexture(0, backTex);


    PassSamplers foreSamplers, backSamplers;
    foreSamplers[0] = foreTex;
    backSamplers[0] = backTex;

    foreQuad = new DrawingQuad();
    foreQuad->SetPos(Vector2f());
    foreQuad->SetSize(Vector2f(1.0f, 1.0f));
    foreQuad->SetDepth(1.0f);
    foreQuad->GetMesh().TextureSamplers.insert(foreQuad->GetMesh().TextureSamplers.end(), foreSamplers);
    foreQuad->GetMesh().FloatUniformValues["brightness"] = Mesh::UniformValue<float>(1.0f);
    backQuad = new DrawingQuad();
    backQuad->SetPos(Vector2f());
    backQuad->SetSize(Vector2f(3.0f, 3.0f));
    backQuad->SetDepth(-1.0f);
    backQuad->GetMesh().TextureSamplers.insert(backQuad->GetMesh().TextureSamplers.end(), backSamplers);
    backQuad->GetMesh().FloatUniformValues["brightness"] = Mesh::UniformValue<float>(1.0f);


    cam = new Camera(Vector3f(0, 0, 5.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f), true);
    cam->MinOrthoBounds = Vector3f(-500.0f, -500.0f, -100.0f);
    cam->MaxOrthoBounds = Vector3f(500.0f, 500.0f, 0.01f);
    cam->Info = ProjectionInfo(ToRadian(55.0f), windowSize.x, windowSize.y, 1.0f, 10000.0f);
}

void TwoDOpenGLTest::OnInitializeError(std::string errorMsg)
{
    EndWorld();

    SFMLOpenGLWorld::OnInitializeError(errorMsg);

    std::cout << "Enter any key to continue:\n";
    Pause();
}

void TwoDOpenGLTest::CleanUp(void)
{
    DeleteAndSetToNull(backMat);
    DeleteAndSetToNull(foreMat);
    DeleteAndSetToNull(foreQuad);
    DeleteAndSetToNull(backQuad);
    DeleteAndSetToNull(cam);
}


void TwoDOpenGLTest::UpdateWorld(float elapsedSeconds)
{
    foreQuad->SetSize(Vector2f(3.0, 1.0f * sinf(GetTotalElapsedSeconds())));

    if (Input.GetBoolInputValue(1))
    {
        cam->SetPositionZ(cam->GetPosition().z - (elapsedSeconds * 25.0f));
        std::cout << cam->GetPosition().z << "\n";
    }
    if (Input.GetBoolInputValue(0))
    {
        cam->SetPositionZ(cam->GetPosition().z + (elapsedSeconds * 25.0f));
        std::cout << cam->GetPosition().z << "\n";
    }

    if (Input.GetBoolInputValue(2))
    {
        float br = foreQuad->GetMesh().FloatUniformValues["brightness"].Data[0];
        br -= 0.5f * elapsedSeconds;
        std::cout << br << "\n";
        foreQuad->GetMesh().FloatUniformValues["brightness"].Data[0] = br;
        backQuad->GetMesh().FloatUniformValues["brightness"].Data[0] = 1.0f - br;
    }
    if (Input.GetBoolInputValue(3))
    {
        float br = foreQuad->GetMesh().FloatUniformValues["brightness"].Data[0];
        br += 0.5f * elapsedSeconds;
        std::cout << br << "\n";
        foreQuad->GetMesh().FloatUniformValues["brightness"].Data[0] = br;
        backQuad->GetMesh().FloatUniformValues["brightness"].Data[0] = 1.0f - br;
    }

    if (Input.GetBoolInputValue(1234))
    {
        EndWorld();
        return;
    }
}

void TwoDOpenGLTest::RenderOpenGL(float elapsedSeconds)
{
    TransformObject trans;
    Matrix4f worldM, viewM, projM;

    trans.GetWorldTransform(worldM);
    cam->GetViewTransform(viewM);
    //cam->GetOrthoProjection(projM);
    projM.SetAsPerspProj(cam->Info);

    RenderInfo info(this, cam, &trans, &worldM, &viewM, &projM);


    ScreenClearer(true, true, true, Vector4f(0.2f, 0.0f, 0.0f, 1.0f)).ClearScreen();

    if (!backQuad->Render(info, *backMat))
    {
        std::cout << "Error rendering background: " << backMat->GetErrorMessage();
        Pause();
        EndWorld();
        return;
    }
    if (!foreQuad->Render(info, *foreMat))
    {
        std::cout << "Error rendering foreground: " << foreMat->GetErrorMessage();
        Pause();
        EndWorld();
        return;
    }
}

void TwoDOpenGLTest::OnWindowResized(unsigned int w, unsigned int h)
{
    glViewport(0, 0, w, h);
    
    cam->Info.Width = w;
    cam->Info.Height = h;
}