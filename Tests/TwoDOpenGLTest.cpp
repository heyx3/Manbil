#include "TwoDOpenGLTest.h"

#include "../ScreenClearer.h"


#include <iostream>
namespace TwoDOpenGLTestStuff
{
    Vector2i windowSize(750, 750);

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

    background.loadFromFile("Water.png");
    foreground.loadFromFile("shrub.png");

    RenderObjHandle foreTex, backTex;
    RenderDataHandler::CreateTexture2D(foreTex, foreground);
    RenderDataHandler::CreateTexture2D(backTex, background);

    PassSamplers foreSamplers, backSamplers;
    foreSamplers[0] = foreTex;
    backSamplers[0] = backTex;

    foreQuad = new DrawingQuad();
    foreQuad->GetMesh().TextureSamplers.insert(foreQuad->GetMesh().TextureSamplers.end(), foreSamplers);
    foreQuad->GetMesh().FloatUniformValues["brightness"] = Mesh::UniformValue<float>(1.0f);
    backQuad = new DrawingQuad();
    backQuad->GetMesh().TextureSamplers.insert(backQuad->GetMesh().TextureSamplers.end(), backSamplers);
    backQuad->GetMesh().FloatUniformValues["brightness"] = Mesh::UniformValue<float>(1.0f);

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


    cam = new Camera(Vector3f(), Vector3f(0.0f, 0.0f, -101.0f), Vector3f(0.0f, 1.0f, 0.0f), true);
    cam->MinOrthoBounds = Vector3f(-500.0f, -500.0f, -100.0f);
    cam->MaxOrthoBounds = -cam->MinOrthoBounds;
    cam->Info = ProjectionInfo(ToRadian(55.0f), 750, 750, 1.0f, 9999.0f);
}

void TwoDOpenGLTest::CleanUp(void)
{
    if (backMat != 0) delete backMat;
    if (foreMat != 0) delete foreMat;
    if (foreQuad != 0) delete foreQuad;
    if (backQuad != 0) delete backQuad;
    if (cam != 0) delete cam;
}


void TwoDOpenGLTest::UpdateWorld(float elapsedSeconds)
{

}

void TwoDOpenGLTest::RenderOpenGL(float elapsedSeconds)
{
    TransformObject trans;
    Matrix4f worldM, viewM, projM, wvpM;

    trans.GetWorldTransform(worldM);
    cam->GetViewTransform(viewM);
    cam->GetOrthoProjection(projM);
    projM.SetAsPerspProj(cam->Info);
    wvpM.SetAsWVP(projM, viewM, worldM);

    RenderInfo info(this, cam, &trans, &worldM, &viewM, &projM);


    ScreenClearer(true, false).ClearScreen();

    backQuad->SetQuadPos(Vector2f());
    backQuad->SetQuadSize(Vector2f(1000.0f, 1000.0f));
    backQuad->SetQuadDepth(-1.0f);
    backQuad->Render(info, *backMat);

    foreQuad->SetQuadPos(Vector2f());
    foreQuad->SetQuadSize(Vector2f(100.0f, 100.0f));
    foreQuad->SetQuadDepth(1.0f);
    foreQuad->Render(info, *foreMat);
}