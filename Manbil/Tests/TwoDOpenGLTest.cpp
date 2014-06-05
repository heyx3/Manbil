#include "TwoDOpenGLTest.h"

#include "../ScreenClearer.h"
#include "../TextureSettings.h"
#include "../Input/Input Objects/KeyboardBoolInput.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"


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
                      sf::ContextSettings(24, 0, 0, 3, 3)),
      cam(0), foreQuad(0), backQuad(0), quadMat(0)
{

}

void TwoDOpenGLTest::InitializeWorld(void)
{
    //Basic OpenGL/world initialization.
    SFMLOpenGLWorld::InitializeWorld();
    GetWindow()->setVerticalSyncEnabled(true);
    GetWindow()->setMouseCursorVisible(true);
    glViewport(0, 0, windowSize.x, windowSize.y);


    //Camera.
    cam = new Camera(Vector3f(0, 0, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
    cam->MinOrthoBounds = Vector3f(-5.0f, -5.0f, -5.0f);
    cam->MaxOrthoBounds = Vector3f(5.0f, 5.0f, 5.0f);
    cam->Info = ProjectionInfo(ToRadian(55.0f), windowSize.x, windowSize.y, 1.0f, 10000.0f);


    //Input.
    Input.AddBoolInput(0, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::E, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(1, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Q, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(2, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Up, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(3, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Down, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(5, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::W, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(6, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::S, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(7, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::A, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(8, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::D, BoolInput::ValueStates::IsDown)));
    Input.AddBoolInput(1234, BoolInputPtr((BoolInput*)new KeyboardBoolInput(sf::Keyboard::Escape, BoolInput::ValueStates::IsDown)));


    //Drawing quads.
    foreQuad = new DrawingQuad();
    foreQuad->SetPos(Vector2f());
    foreQuad->SetSize(Vector2f(1.0f, 1.0f));
    foreQuad->SetDepth(1.0f);
    backQuad = new DrawingQuad();
    backQuad->SetPos(Vector2f());
    backQuad->SetSize(Vector2f(3.0f, 3.0f));
    backQuad->SetDepth(-1.0f);


    //Textures.
    foreTex = Textures.CreateTexture("Content/Textures/shrub.png");
    if (foreTex == TextureManager::UNUSED_ID)
    {
        std::cout << "Error loading 'Content/Textures/shrub.png'\n";
        Pause();
        EndWorld();
        return;
    }
    backTex = Textures.CreateTexture("Content/Textures/Water.png");
    if (backTex == TextureManager::UNUSED_ID)
    {
        std::cout << "Error loading 'Content/Textures/Water.png'\n";
        Pause();
        EndWorld();
        return;
    }

    TextureSettings setts(TextureSettings::TF_LINEAR, TextureSettings::TW_CLAMP, true);
    Textures[foreTex].SetData(setts);
    Textures[backTex].SetData(setts);


    //Fonts.

    std::string err = GetCurrentRenderingError();
    if (!err.empty())
    {
        std::cout << "Error creating textures: " << err.c_str();
        Pause();
        EndWorld();
        return;
    }


    //Materials.

    std::unordered_map<RenderingChannels, DataLine> channels;
    channels[RenderingChannels::RC_VertexPosOutput] = DataNodeGenerators::ObjectPosToScreenPos<VertexPosTex1Normal>(0);
    channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(DataNodePtr(new VertexInputNode(DrawingQuad::GetAttributeData())), 1);
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new TextureSampleNode(DataLine(DataNodePtr(new FragmentInputNode(VertexAttributes(2, false))), 0), "u_myTex")),
                                                     TextureSampleNode::GetOutputIndex(ChannelsOut::CO_AllColorChannels));

    UniformDictionary uniformDict;
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(channels, uniformDict, DrawingQuad::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
    if (!genM.ErrorMessage.empty())
    {
        std::cout << "Error generating quad mat shaders: " << genM.ErrorMessage;
        Pause();
        EndWorld();
        return;
    }
    quadMat = genM.Mat;
    if (quadMat->HasError())
    {
        std::cout << "Error creating quad mat: " << quadMat->GetErrorMsg();
        Pause();
        EndWorld();
        return;
    }

    foreParam.AddUniforms(uniformDict, false);
    foreParam.TextureUniforms["u_myTex"].Texture = Textures[foreTex];
    backParam.AddUniforms(uniformDict, false);
    backParam.TextureUniforms["u_myTex"].Texture = Textures[backTex];
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
    DeleteAndSetToNull(quadMat);
    DeleteAndSetToNull(foreQuad);
    DeleteAndSetToNull(backQuad);
    DeleteAndSetToNull(cam);
}


void TwoDOpenGLTest::UpdateWorld(float elapsedSeconds)
{
    if (Input.GetBoolInputValue(1))
    {
        cam->SetPositionZ(cam->GetPosition().z - (elapsedSeconds * 25.0f));
    }
    if (Input.GetBoolInputValue(0))
    {
        cam->SetPositionZ(cam->GetPosition().z + (elapsedSeconds * 25.0f));
    }
    if (Input.GetBoolInputValue(5))
    {
        cam->SetPositionY(cam->GetPosition().y + (elapsedSeconds * 25.0f));
    }
    if (Input.GetBoolInputValue(6))
    {
        cam->SetPositionY(cam->GetPosition().y - (elapsedSeconds * 25.0f));
    }
    if (Input.GetBoolInputValue(7))
    {
        cam->SetPositionX(cam->GetPosition().x + (elapsedSeconds * 25.0f));
    }
    if (Input.GetBoolInputValue(8))
    {
        cam->SetPositionX(cam->GetPosition().x - (elapsedSeconds * 25.0f));
    }

    if (Input.GetBoolInputValue(2))
    {
        foreQuad->SetSize(Vector2f(foreQuad->GetMesh().Transform.GetScale().x * 1.01f,
                                   foreQuad->GetMesh().Transform.GetScale().y * 1.01f));
        backQuad->SetSize(Vector2f(backQuad->GetMesh().Transform.GetScale().x * 1.01f,
                                   backQuad->GetMesh().Transform.GetScale().y * 1.01f));
    }
    if (Input.GetBoolInputValue(3))
    {
        foreQuad->SetSize(Vector2f(foreQuad->GetMesh().Transform.GetScale().x * 0.99f,
                                   foreQuad->GetMesh().Transform.GetScale().y * 0.99f));
        backQuad->SetSize(Vector2f(backQuad->GetMesh().Transform.GetScale().x * 0.99f,
                                   backQuad->GetMesh().Transform.GetScale().y * 0.99f));
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
    cam->GetOrthoProjection(projM);

    RenderInfo info(this, cam, &trans, &worldM, &viewM, &projM);
    RenderingState().EnableState();


    ScreenClearer(true, true, false, Vector4f(0.1f, 0.0f, 0.0f, 1.0f)).ClearScreen();

    if (!backQuad->Render(RenderPasses::BaseComponents, info, backParam, *quadMat))
    {
        std::cout << "Error rendering background: " << quadMat->GetErrorMsg();
        Pause();
        EndWorld();
        return;
    }
    if (!foreQuad->Render(RenderPasses::BaseComponents, info, foreParam, *quadMat))
    {
        std::cout << "Error rendering foreground: " << quadMat->GetErrorMsg();
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