#include "GUITestWorld.h"

#include <stdio.h>
#include <wchar.h>

#include "../Vertices.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"

#include "../ScreenClearer.h"
#include "../RenderingState.h"


//Debugging/error-printing.
#include <iostream>
namespace GUITESTWORLD_NAMESPACE
{
    void Pause(void)
    {
        char dummy;
        std::cin >> dummy;
    }
    bool PrintError(bool whetherToPrint, std::string errorIntro, std::string errorMsg)
    {
        if (whetherToPrint)
        {
            std::cout << errorIntro << ": " << errorMsg << "\n";
        }

        return whetherToPrint;
    }
}
using namespace GUITESTWORLD_NAMESPACE;


Vector2i GUITestWorld::WindowSize = Vector2i(600, 600);
std::string textSamplerName = "u_textSampler";



#pragma region My TextRenderer code

#include "../Rendering/GUI/TextRenderer.h"


unsigned int textRendererID = FreeTypeHandler::ERROR_ID;
const Vector2i textSize(512, 64);


//Returns an error message, or an empty string if everything went fine.
std::string LoadFont(TextRenderer * rendr, std::string fontPath, unsigned int size)
{
    if (textRendererID != FreeTypeHandler::ERROR_ID)
        return "'textRendererID' was already set to " + std::to_string(textRendererID);

    textRendererID = rendr->CreateAFont(fontPath, 50);
    if (textRendererID == FreeTypeHandler::ERROR_ID)
    {
        return "Error creating font '" + fontPath + "': " + rendr->GetError();
    }
    if (!rendr->CreateTextRenderSlots(textRendererID, textSize.x, textSize.y, false, TextureSettings(TextureSettings::FT_LINEAR, TextureSettings::WT_CLAMP)))
    {
        return "Error creating render slot for '" + fontPath + "': " + rendr->GetError();
    }

    return "";
}
//Returns an error message, or an empty string if everything went fine.
std::string RenderText(TextRenderer * rendr, std::string text)
{
    if (textRendererID == FreeTypeHandler::ERROR_ID)
        return "'textRendererID' wasn't set to anything";

    if (!rendr->RenderString(TextRenderer::FontSlot(textRendererID, 0), text, GUITestWorld::WindowSize.x, GUITestWorld::WindowSize.y))
        return "Error rendering string '" + text + "': " + rendr->GetError();

    return "";
}


#pragma endregion




bool GUITestWorld::ReactToError(bool isEverythingOK, std::string errorIntro, std::string errorMsg)
{
    if (PrintError(!isEverythingOK, errorIntro, errorMsg))
    {
        Pause();
        EndWorld();
    }

    return isEverythingOK;
}
void GUITestWorld::OnInitializeError(std::string errorMsg)
{
    SFMLOpenGLWorld::OnInitializeError(errorMsg);
    Pause();
    EndWorld();
}


void GUITestWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    WindowSize.x = newW;
    WindowSize.y = newH;
    glViewport(0, 0, newW, newH);
}


void GUITestWorld::InitializeWorld(void)
{
    std::string err;


    SFMLOpenGLWorld::InitializeWorld();

    //Initialize static stuff.
    err = InitializeStaticSystems(false, true, true);
    if (!ReactToError(err.empty(), "Error initializing static systems", err))
        return;


    //Create the drawing quad.
    quad = new DrawingQuad();

    //Scale the window size according to the text dimensions.
    if (textSize.x > textSize.y)
    {
        WindowSize.y = (int)(WindowSize.x * (float)textSize.y / (float)textSize.x);
    }
    else
    {
        WindowSize.x = (int)(WindowSize.y * (float)textSize.x / (float)textSize.y);
    }

    GetWindow()->setSize(sf::Vector2u(WindowSize.x, WindowSize.y));


    //Create the quad rendering material.
    std::unordered_map<RenderingChannels, DataLine> channels;
    DataNodePtr vertexIns(new VertexInputNode(DrawingQuad::GetAttributeData()));
    DataLine worldPos(DataNodePtr(new ObjectPosToWorldPosCalcNode(DataLine(vertexIns, 0))), 0);
    channels[RenderingChannels::RC_VertexPosOutput] = DataLine(DataNodePtr(new CombineVectorNode(worldPos, DataLine(1.0f))), 0);
    channels[RenderingChannels::RC_VERTEX_OUT_0] = DataLine(vertexIns, 1);
    //The text texture only stores the red component (to save space).
    DataLine redText(DataNodePtr(new TextureSampleNode(DataLine(DataNodePtr(new FragmentInputNode(VertexAttributes(2, false))), 0),
                                                       textSamplerName)),
                     TextureSampleNode::GetOutputIndex(ChannelsOut::CO_Red));
    channels[RenderingChannels::RC_Color] = DataLine(DataNodePtr(new CombineVectorNode(redText, redText, redText)), 0);
    channels[RenderingChannels::RC_Opacity] = redText;
    ShaderGenerator::GeneratedMaterial genMat = ShaderGenerator::GenerateMaterial(channels, quadParams, DrawingQuad::GetAttributeData(), RenderingModes::RM_Opaque, false, LightSettings(false));
    if (!ReactToError(genMat.ErrorMessage.empty(), "Error generating quad material", genMat.ErrorMessage))
        return;
    quadMat = genMat.Mat;


    //Load the font.
    err = LoadFont(TextRender, "Content/Fonts/Candara.ttf", 25);
    if (!ReactToError(err.empty(), "Error loading 'Content/Fonts/Candara.ttf'", err))
        return;

    //Render a string.
    err = RenderText(TextRender, "Hello, World!!");
    if (!ReactToError(err.empty(), "Error rendering the text: ", err))
        return;
    quadParams.Texture2DUniforms[textSamplerName].Texture = TextRender->GetRenderedString(textRendererID);
}
void GUITestWorld::DestroyMyStuff(bool destroyStatics)
{
    DeleteAndSetToNull(quad);
    DeleteAndSetToNull(quadMat);

    if (destroyStatics) DestroyStaticSystems(false, true, true);
}


void GUITestWorld::UpdateWorld(float elapsed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        EndWorld();
}
void GUITestWorld::RenderOpenGL(float elapsed)
{
    //Prepare the back-buffer to be rendered into.
    ScreenClearer().ClearScreen();
    RenderingState(false, false, RenderingState::C_NONE).EnableState();

    //Set up the "render info" struct.
    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
    cam.MinOrthoBounds = Vector3f(-1.0f, -1.0f, -1.0f);
    cam.MaxOrthoBounds = Vector3f(1.0f, 1.0f, 1.0f);
    cam.Info.Width = WindowSize.x;
    cam.Info.Height = WindowSize.y;
    Matrix4f worldM, viewM, projM;
    quad->GetMesh().Transform.GetWorldTransform(worldM);
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);
    RenderInfo info(this, &cam, &quad->GetMesh().Transform, &worldM, &viewM, &projM);

    //Render the quad.
    if (!ReactToError(quad->Render(RenderPasses::BaseComponents, info, quadParams, *quadMat), "Error rendering quad", quadMat->GetErrorMsg()))
        return;
}