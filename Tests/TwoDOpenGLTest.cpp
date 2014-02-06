#include "TwoDOpenGLTest.h"

#include "../ScreenClearer.h"
#include "../TextureSettings.h"
#include "../Input/Input Objects/KeyboardBoolInput.h"


#include <iostream>
namespace TwoDOpenGLTestStuff
{
    Vector2i windowSize(1000, 700);

    void Pause()
    {
        char dummy;
        std::cin >> dummy;
    }
}
using namespace TwoDOpenGLTestStuff;



TwoDOpenGLTest::TwoDOpenGLTest(void)
    : SFMLOpenGLWorld(750, 750, sf::ContextSettings()), cam(0), foreQuad(0), backQuad(0), backMat(0), foreMat(0)
{

}

void TwoDOpenGLTest::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    glViewport(0, 0, windowSize.x, windowSize.y);

    Input.AddBoolInput(1, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::W, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(0, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::S, BoolInput::ValueStates::IsDown)));


    //RenderingState().EnableState();

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

    TextureSettings setts(TextureSettings::TF_LINEAR, TextureSettings::TW_CLAMP, false);
    RenderObjHandle foreTex, backTex;

    RenderDataHandler::CreateTexture2D(foreTex, foreground);
    setts.SetData(foreTex);
    RenderDataHandler::CreateTexture2D(backTex, background);
    setts.SetData(backTex);


    PassSamplers foreSamplers, backSamplers;
    foreSamplers[0] = foreTex;
    backSamplers[0] = backTex;

    foreQuad = new DrawingQuad();
    foreQuad->GetMesh().TextureSamplers.insert(foreQuad->GetMesh().TextureSamplers.end(), foreSamplers);
    foreQuad->GetMesh().FloatUniformValues["brightness"] = Mesh::UniformValue<float>(1.0f);
    foreQuad->SetPos(Vector2f());
    backQuad = new DrawingQuad();
    backQuad->GetMesh().TextureSamplers.insert(backQuad->GetMesh().TextureSamplers.end(), backSamplers);
    backQuad->GetMesh().FloatUniformValues["brightness"] = Mesh::UniformValue<float>(1.0f);
    backQuad->SetPos(Vector2f());

    backMat = new Material(Materials::UnlitTexture);
    if (backMat->HasError())
    {
        std::cout << "Error creating background mat: " << backMat->GetErrorMessage();
        Pause();
        EndWorld();
        return;
    }
    backMat->AddUniform("brightness");
    foreMat = new Material(Materials::UnlitTexture);
    if (foreMat->HasError())
    {
        std::cout << "Error creating foreground mat: " << foreMat->GetErrorMessage();
        Pause();
        EndWorld();
        return;
    }
    foreMat->AddUniform("brightness");


    cam = new Camera(Vector3f(0, 0, 5.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f), true);
    cam->MinOrthoBounds = Vector3f(-500.0f, -500.0f, -100.0f);
    cam->MaxOrthoBounds = Vector3f(500.0f, 500.0f, 0.01f);
    cam->Info = ProjectionInfo(ToRadian(55.0f), windowSize.x, windowSize.y, 1.0f, 10000.0f);
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
    foreQuad->SetSize(Vector2f(3.0, 10.0f * sinf(GetTotalElapsedSeconds())));

    if (Input.GetBoolInputValue(0))
    {
        cam->SetPositionZ(cam->GetPosition().z - (elapsedSeconds * 25.0f));
        std::cout << cam->GetPosition().z << "\n";
    }
    if (Input.GetBoolInputValue(1))
    {
        cam->SetPositionZ(cam->GetPosition().z + (elapsedSeconds * 25.0f));
        std::cout << cam->GetPosition().z << "\n";
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


    ScreenClearer().ClearScreen();

    //if (!backQuad->Render(info, *backMat))
    //{
    //    std::cout << "Error rendering background: " << backMat->GetErrorMessage();
    //    Pause();
    //    EndWorld();
    //    return;
    //}
    if (!foreQuad->Render(info, *foreMat))
    {
        std::cout << "Error rendering foreground: " << backMat->GetErrorMessage();
        Pause();
        EndWorld();
        return;
    }
}