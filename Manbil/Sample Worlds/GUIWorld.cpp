#include "GUIWorld.h"

#include "../Rendering/Rendering.hpp"
#include "../Rendering/GUI/GUIMaterials.h"
#include "../Rendering/GUI/GUI Elements/GUICheckbox.h"
#include "../Rendering/GUI/GUI Elements/GUITextBox.h"
#include "../Rendering/GUI/GUI Elements/GUISlider.h"
#include "../Rendering/GUI/GUI Elements/GUIFormattedPanel.h"

#include "../Rendering/Data Nodes/DataNodes.hpp"


#include <iostream>
//This is a little helper function that is used whenver an error is being printed to the console.
void PauseConsole(void)
{
    char dummy;
    std::cin >> dummy;
}


//The GUI's camera is positioned so the origin is at the bottom-left of the window,
//    and the width/height of the view is equal to the pixel width/height.
Camera GetGUICam(Vector2u windowSize)
{
    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, -1.0f, 0.0f));
    cam.MinOrthoBounds = Vector3f(0.0f, 0.0f, -10.0f);
    cam.MaxOrthoBounds = Vector3f((float)windowSize.x, (float)windowSize.y, 10.0f);
    cam.PerspectiveInfo.Width = windowSize.x;
    cam.PerspectiveInfo.Height = windowSize.y;
    return cam;
}


GUIWorld::GUIWorld(void)
    : windowSize(800, 600), SFMLOpenGLWorld(800, 600, sf::ContextSettings()),
      texBackground(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_8U, false),
      texCheck(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_8U, false),
      texSliderBar(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_8U, false),
      texSliderNub(TextureSampleSettings2D(FT_LINEAR, WT_CLAMP), PixelSizes::PS_8U, false),
      simpleGUIMat(0), animatedGUIMat(0), guiTextMat(0)
{

}


sf::VideoMode GUIWorld::GetModeToUse(unsigned int windowW, unsigned int windowH)
{
    return sf::VideoMode(windowW, windowH);
}
std::string GUIWorld::GetWindowTitle(void)
{
    return "GUIWorld";
}
sf::Uint32 GUIWorld::GetSFStyleFlags(void)
{
    return sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
}


void GUIWorld::InitializeTextures(void)
{
    const std::string contentPath = "Content/Default Editor Content/";
    
    texBackground.Create();
    texCheck.Create();
    texSliderBar.Create();
    texSliderNub.Create();

    std::string error;
    texBackground.SetDataFromFile("Content/Default Editor Content/CheckboxBackground.png", error);
    if (!error.empty())
    {
        std::cout << "Error loading background tex: " << error << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }
    texCheck.SetDataFromFile("Content/Default Editor Content/CheckboxCheck.png", error);
    if (!error.empty())
    {
        std::cout << "Error loading checkbox tex: " << error << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }
    texSliderBar.SetDataFromFile("Content/Sample Worlds/GUIBar.png", error);
    if (!error.empty())
    {
        std::cout << "Error loading slider bar tex: " << error << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }
    texSliderNub.SetDataFromFile("Content/Sample Worlds/GUINub.png", error);
    if (!error.empty())
    {
        std::cout << "Error loading slider nub tex: " << error << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }
}
void GUIWorld::InitializeMaterials(void)
{
    //The "GUIMaterials" class provides everything we need to create most GUI materials.
    //Just like with ShaderGenerator, we are responsible for managing a material's memory
    //    after generating it using this class.

    //If you wish to create your own GUI material from scratch, just make sure to do the following:
    //    1) Use the DrawingQuad's vertex attributes -- all GUIElements use DrawingQuad to render.
    //    2) Expose the vec4 color uniform "GUIMaterials::QuadDraw_Color" as a color blend value.
    //    3) Expose the texture uniform "GUIMaterials::QuadDraw_Texture2D" as a base texture value
    //         (and multiply it by "GUIMaterials::QuadDraw_Color").
    //    4) If you want to animate your material, also expose the float uniform
    //         "GUIMaterials::DynamicQuadDraw_TimeLerp", which ranges between 0 (untouched) and 1
    //         (fully tweened).
    //    5) Keep in mind that text rendering is done with greyscale textures, which store their
    //         values in the Red component.
    
    GUIMaterials::GenMat tryMat("");

    //First, create the simple unanimated material.
    tryMat = GUIMaterials::GenerateStaticQuadDrawMaterial(simpleGUIMatParams, GUIMaterials::TT_COLOR);
    if (!tryMat.ErrorMessage.empty())
    {
        std::cout << "Error creating simple GUI material: " << tryMat.ErrorMessage << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }
    simpleGUIMat = tryMat.Mat;

    //Next, create the text material.
    tryMat = GUIMaterials::GenerateStaticQuadDrawMaterial(guiTextMatParams, GUIMaterials::TT_TEXT);
    if (!tryMat.ErrorMessage.empty())
    {
        std::cout << "Error creating text GUI material: " << tryMat.ErrorMessage << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }
    guiTextMat = tryMat.Mat;


    //Finally, set up the animated material.
    //The animation is controlled by a single parameter that moves between 0 and 1.
    DataNode::Ptr lerpParam(new ParamNode(1, GUIMaterials::DynamicQuadDraw_TimeLerp));
    DataNode::Ptr animatedColor(new InterpolateNode(Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
                                                    Vector4f(0.5f, 0.5f, 0.5f, 1.0f),
                                                    lerpParam, InterpolateNode::IT_Linear,
                                                    "animatedColor")),
                  animatedSize(new InterpolateNode(1.0f, 1.2f, lerpParam,
                                                   InterpolateNode::IT_VerySmooth,
                                                   "animatedSize"));
    DataNode::Ptr animatedSize2D(new CombineVectorNode(animatedSize, animatedSize, "animatedSize2D"));
    tryMat = GUIMaterials::GenerateDynamicQuadDrawMaterial(animatedGUIMatParams,
                                                           GUIMaterials::TT_COLOR,
                                                           animatedSize2D, animatedColor);
    if (!tryMat.ErrorMessage.empty())
    {
        std::cout << "Error creating animated GUI material: " << tryMat.ErrorMessage << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }
    animatedGUIMat = tryMat.Mat;
}
void GUIWorld::InitializeGUI(void)
{
    std::string errorMsg;


    //First set up the text renderer and load the font.
    textRenderer = new TextRenderer();
    textFont = textRenderer->CreateAFont("Content/Default Editor Content/Inconsolata.otf",
                                         errorMsg, 75);
    if (textFont == FreeTypeHandler::ERROR_ID)
    {
        std::cout << "Error loading font for rendering: " << errorMsg << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }

    //Next, create a "font slot" to render text into.
    TextureSampleSettings2D fontRenderSettings(FT_LINEAR, WT_CLAMP);
    if (!textRenderer->CreateTextRenderSlots(textFont, errorMsg, 1024, 256,
                                             false, fontRenderSettings))
    {
        std::cout << "Error creating text render slot: " << errorMsg << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }
    //Font slots are numbered in order starting at 0, and we're only using one slot.
    TextRenderer::FontSlot textSlot(textFont, 0);


    //Next, create the GUI items.

    GUILabel* title = new GUILabel(guiTextMatParams, textRenderer, textSlot, guiTextMat);
    if (!title->SetText("WASD:Move  EQ:Zoom"))
    {
        std::cout << "Failed to set GUI label's text\n";
        PauseConsole();
        EndWorld();
        return;
    }
    title->ScaleBy(Vector2f(0.3f, 0.3f));
    title->SetColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
    title->Depth = 0.001f;

    GUITexture sliderBar(simpleGUIMatParams, &texSliderBar, simpleGUIMat),
               sliderNub(animatedGUIMatParams, &texSliderNub, animatedGUIMat, true, 1.0f);
    sliderNub.ScaleBy(Vector2f(0.5f, 0.5f));
    sliderNub.Depth = 0.001f;
    GUISlider* slider = new GUISlider(UniformDictionary(), sliderBar, sliderNub, 0.5f);
    slider->OnValueChanged = [](GUISlider* slider, Vector2f mousePos, void* userData)
    {
        std::cout << "Slider: " << slider->Value << "\n";
    };
    slider->ScaleBy(Vector2f(6.0f, 5.0f));
    slider->Depth = 0.001f;

    GUITexture checkboxBackground(animatedGUIMatParams, &texBackground, animatedGUIMat, true, 2.0f),
               checkboxForeground(simpleGUIMatParams, &texCheck, simpleGUIMat);
    GUICheckbox* checkbox = new GUICheckbox(checkboxBackground, checkboxForeground, false);
    checkbox->Depth = 0.001f;

    
    //Create a "GUIFormattedPanel", which is just a GUIElement that holds other GUIElements.
    //Unlike the "GUIPanel", "GUIFormattedPanel" will automatically fit the elements together.
    GUITexture panelBackground(simpleGUIMatParams, &texBackground, simpleGUIMat);
    GUIFormattedPanel* panel = new GUIFormattedPanel(40.0f, 40.0f, panelBackground);
    panel->AddObject(GUIFormatObject(GUIElementPtr(title), false, true, Vector2f(10.0f, 20.0f)));
    panel->AddObject(GUIFormatObject(GUIElementPtr(slider), false, true, Vector2f(0.0f, 10.0f)));
    panel->AddObject(GUIFormatObject(GUIElementPtr(checkbox), false, true, Vector2f()));
    guiManager.RootElement = GUIElementPtr(panel);

    //Finally, position the panel so it's at the bottom-left corner of the screen.
    Box2D bounds = guiManager.RootElement->GetBounds();
    guiManager.RootElement->SetBounds(Box2D(0.0f, bounds.GetXSize(),
                                            -bounds.GetYSize(), 0.0f));
}

void GUIWorld::InitializeWorld(void)
{
    SFMLOpenGLWorld::InitializeWorld();

    //If there was an error initializing the window/OpenGL, don’t bother with
    //    the rest of initialization.
    if (IsGameOver())
    {
        return;
    }


    DrawingQuad::InitializeQuad();

    std::string err;
    err = TextRenderer::InitializeSystem();
    if (!err.empty())
    {
        std::cout << "Error initializing text rendering system: " << err << "\n\n";
        PauseConsole();
        EndWorld();
        return;
    }


    InitializeTextures();
    InitializeMaterials();
    InitializeGUI();
}
void GUIWorld::OnWorldEnd(void)
{
    guiManager.RootElement.reset();

    delete textRenderer;
    textFont = 0;

    if (simpleGUIMat != 0)
    {
        delete simpleGUIMat;
    }
    if (animatedGUIMat != 0)
    {
        delete animatedGUIMat;
    }
    if (guiTextMat != 0)
    {
        delete guiTextMat;
    }
    
    texBackground.DeleteIfValid();
    texCheck.DeleteIfValid();
    texSliderBar.DeleteIfValid();
    texSliderNub.DeleteIfValid();

    DrawingQuad::DestroyQuad();
    TextRenderer::DestroySystem();
}


void GUIWorld::UpdateWorld(float elapsedSeconds)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
    }
    
    //Move the gui window with WASD.
    const float speed = 100.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        guiManager.RootElement->MoveElement(Vector2f(-(speed * elapsedSeconds), 0.0f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        guiManager.RootElement->MoveElement(Vector2f((speed * elapsedSeconds), 0.0f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        guiManager.RootElement->MoveElement(Vector2f(0.0f, -(speed * elapsedSeconds)));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        guiManager.RootElement->MoveElement(Vector2f(0.0f, (speed * elapsedSeconds)));
    }

    //Scale the gui window with EQ.
    const float zoom = 1.01f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
    {
        guiManager.RootElement->ScaleBy(Vector2f(zoom, zoom));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
    {
        guiManager.RootElement->ScaleBy(Vector2f(1.0f / zoom, 1.0f / zoom));
    }

    //Calculate the mouse position relative to the window.
    sf::Vector2i mPos = sf::Mouse::getPosition();
    sf::Vector2i mPosFinal = mPos - GetWindow()->getPosition() - sf::Vector2i(5, 30);
    mPosFinal.y -= windowSize.y;

    //Update the GUI manager.
    guiManager.Update(elapsedSeconds, Vector2i(mPosFinal.x, mPosFinal.y),
                      sf::Mouse::isButtonPressed(sf::Mouse::Left));
}
void GUIWorld::RenderOpenGL(float elapsedSeconds)
{
    //Set up rendering state.
    ScreenClearer(true, true, false, Vector4f(0.2, 0.2, 0.3f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_BACK, false, true).EnableState();
    glViewport(0, 0, windowSize.x, windowSize.y);

    //Set up the render camera.
    Camera cam = GetGUICam(windowSize);
    Matrix4f viewM, projM;
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);

    //Render the GUI.
    RenderInfo info(GetTotalElapsedSeconds(), &cam, &viewM, &projM);
    guiManager.Render(elapsedSeconds, info);
}


void GUIWorld::OnInitializeError(std::string errorMsg)
{
    //Print the error message and end the game.
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    PauseConsole();
    EndWorld();
}
void GUIWorld::OnWindowResized(unsigned int newWidth, unsigned int newHeight)
{
    windowSize.x = newWidth;
    windowSize.y = newHeight;
}