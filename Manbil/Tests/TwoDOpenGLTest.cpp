#include "TwoDOpenGLTest.h"

#include "../ScreenClearer.h"
#include "../Input/Input Objects/KeyboardBoolInput.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../Rendering/GUI/TextRenderer.h"


//Debug printing stuff.
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
                      sf::ContextSettings(24, 0, 0, 4, 1)),
      cam(0), foreQuad(0), backQuad(0), quadMat(0),
      foreTex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_8U, true),
      backTex(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PS_8U, true)
{

}

void TwoDOpenGLTest::InitializeWorld(void)
{
    //Basic OpenGL/world initialization.
    SFMLOpenGLWorld::InitializeWorld();

    std::string error = GetCurrentRenderingError();
    if (!error.empty())
    {
        std::cout << "Error initializing SFML/OpenGL: " << error;
        Pause();
        EndWorld();
        return;
    }

    InitializeStaticSystems(false, true, true);

    error = GetCurrentRenderingError();
    if (!error.empty())
    {
        std::cout << "Unknown error initializing static systems: " << error;
        Pause();
        EndWorld();
        return;
    }

    GetWindow()->setVerticalSyncEnabled(true);
    GetWindow()->setMouseCursorVisible(true);
    glViewport(0, 0, windowSize.x, windowSize.y);
    error = GetCurrentRenderingError();
    if (!error.empty())
    {
        std::cout << "Unknown error setting up window settings: " << error;
        Pause();
        EndWorld();
        return;
    }


    //Camera.
    cam = new Camera(Vector3f(0, 0, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
    cam->MinOrthoBounds = Vector3f(-5.0f, -5.0f, -5.0f);
    cam->MaxOrthoBounds = Vector3f(5.0f, 5.0f, 5.0f);
    cam->Info = ProjectionInfo(BasicMath::DegToRad(55.0f), windowSize.x, windowSize.y, 1.0f, 10000.0f);


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

    error = GetCurrentRenderingError();
    if (!error.empty())
    {
        std::cout << "Unknown error initializing simple objects: " << error;
        Pause();
        EndWorld();
        return;
    }
    
    //Create the textures.
    foreTex.Create();
    backTex.Create();
    error = GetCurrentRenderingError();
    if (!error.empty())
    {
        std::cout << "Unknown error creating/clearing textures: " << error;
        Pause();
        EndWorld();
        return;
    }

    //Load the foreground texture as a greyscale texture.
    Array2D<Vector4b> imgData(1, 1);
    Array2D<unsigned char> imgGreyscale(1, 1);
    if (!MTexture2D::LoadImageFromFile("Content/Textures/shrub.png", imgData))
    {
        std::cout << "Error loading 'Content/Textures/shrub.png': " + error + "\n";
        Pause();
        EndWorld();
        return;
    }
    imgGreyscale.Reset(imgData.GetWidth(), imgData.GetHeight());
    imgGreyscale.FillFunc([&imgData](Vector2u loc, unsigned char * outP) { *outP = imgData[loc].y; });
    if (!foreTex.SetGreyscaleData(imgGreyscale, PixelSizes::PS_8U_GREYSCALE))
    {
        std::cout << "Error setting foreground texture data.\n";
        Pause();
        EndWorld();
        return;
    }

    //Load the background texture normally.
    if (!backTex.SetDataFromFile("Content/Textures/Water.png", error))
    {
        std::cout << "Error loading 'Content/Textures/Water.png': " + error + "\n";
        Pause();
        EndWorld();
        return;
    }


    //Materials.

    typedef DataNode::Ptr DNP;

    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetAttributeData();

    DataNode::CurrentShader = ShaderHandler::SH_Vertex_Shader;
    DNP objPosToScreen(new SpaceConverterNode(DataLine(VertexInputNode::GetInstance()),
                                              SpaceConverterNode::ST_OBJECT, SpaceConverterNode::ST_SCREEN,
                                              SpaceConverterNode::DT_POSITION, "objToScreenPos"));
    DataNode::MaterialOuts.VertexPosOutput = DataLine(objPosToScreen, 1);

    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_UV", DataLine(VertexInputNode::GetInstance(), 1)));

    DataNode::CurrentShader = ShaderHandler::SH_Fragment_Shader;
    DNP texSamplePtr(new TextureSample2DNode(DataLine(FragmentInputNode::GetInstance()), "u_myTex", "texSampler"));
    DataLine texSample(texSamplePtr->GetName(), TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
    DNP finalColor(new CombineVectorNode(DataLine(texSample), DataLine(1.0f), "finalColor"));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_FinalColor", DataLine(finalColor)));

    UniformDictionary uniformDict;
    ShaderGenerator::GeneratedMaterial genM = ShaderGenerator::GenerateMaterial(uniformDict, RenderingModes::RM_Opaque);
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
    foreParam.Texture2DUniforms["u_myTex"].Texture = foreTex.GetTextureHandle();
    backParam.AddUniforms(uniformDict, false);
    backParam.Texture2DUniforms["u_myTex"].Texture = backTex.GetTextureHandle();
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
    foreTex.DeleteIfValid();
    backTex.DeleteIfValid();
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

    if (!backQuad->Render(info, backParam, *quadMat))
    {
        std::cout << "Error rendering background: " << quadMat->GetErrorMsg();
        Pause();
        EndWorld();
        return;
    }
    if (!foreQuad->Render(info, foreParam, *quadMat))
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