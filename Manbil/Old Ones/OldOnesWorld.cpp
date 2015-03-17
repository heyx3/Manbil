#include "OldOnesWorld.h"

#include <iostream>
#include "../IO/XmlSerialization.h"
#include "../Rendering/Primitives/DrawingQuad.h"
#include "../Rendering/Data Nodes/DataNodes.hpp"


const float SuperSamplingScale = 1.0f;
const Vector2u GetWorldRenderSize(Vector2u windowSize)
{
    return Vector2u(Mathf::RoundToUInt((float)windowSize.x * SuperSamplingScale),
                    Mathf::RoundToUInt((float)windowSize.y * SuperSamplingScale));
}


OldOnesWorld::OldOnesWorld(void)
    : windowSize(800, 600), renderSize(GetWorldRenderSize(windowSize)),
      SFMLOpenGLWorld(800, 600, sf::ContextSettings()),
      worldRT(0), finalRenderMat(0), ppEffects(0),
      worldColor(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_16U, false),
      worldDepth(TextureSampleSettings2D(FT_NEAREST, WT_CLAMP), PixelSizes::PS_32F_DEPTH, false)
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
    std::string err;

    SFMLOpenGLWorld::InitializeWorld();
    //If there was an error initializing the game, don’t bother with
    //    the rest of initialization.
    if (IsGameOver())
    {
        return;
    }


    DrawingQuad::InitializeQuad();

    ppEffects = new PostProcessing(windowSize, err);
    if (!err.empty())
    {
        std::cout << "Error creating post-process stuff: " << err;
        char dummy;
        std::cin >> dummy;
        EndWorld();
        return;
    }

    skybox = new OldOnesSkybox(err);
    if (!err.empty())
    {
        std::cout << "Error creating skybox: " << err;
        char dummy;
        std::cin >> dummy;
        EndWorld();
        return;
    }


    //Set up world render target.
    worldColor.Create();
    worldColor.ClearData(renderSize.x, renderSize.y);
    worldDepth.Create();
    worldDepth.ClearData(renderSize.x, renderSize.y);
    worldRT = new RenderTarget(PixelSizes::PS_16U_DEPTH, err);
    if (!err.empty())
    {
        std::cout << "Error setting up world render target: " << err << "\n";
        char dummy;
        std::cin >> dummy;
        EndWorld();
        return;
    }
    if (!worldRT->SetColorAttachment(RenderTargetTex(&worldColor), false) ||
        !worldRT->SetDepthAttachment(RenderTargetTex(&worldDepth), true))
    {
        std::cout << "Couldn't set color/depth attachments for world render target\n";
        char dummy;
        std::cin >> dummy;
        EndWorld();
        return;
    }


    //Set up final render material.
    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetVertexInputData();
    DataLine vIn_Pos(VertexInputNode::GetInstance(), 0),
             vIn_UV(VertexInputNode::GetInstance(), 1);
    DataNode::Ptr vOutPos(new CombineVectorNode(vIn_Pos, 1.0f, "vOut_Pos"));
    DataNode::MaterialOuts.VertexPosOutput = vOutPos;
    DataNode::MaterialOuts.VertexOutputs.push_back(ShaderOutput("fIn_UV", vIn_UV));
    DataLine fIn_UV(FragmentInputNode::GetInstance(), 0);
    DataNode::Ptr texSampler(new TextureSample2DNode(fIn_UV, "u_tex", "texSampler"));
    DataLine texRGB(texSampler, TextureSample2DNode::GetOutputIndex(CO_AllColorChannels));
    DataNode::Ptr finalColor(new CombineVectorNode(texRGB, 1.0f, "finalColorNode"));
    DataNode::MaterialOuts.FragmentOutputs.push_back(ShaderOutput("fOut_Color", finalColor));
    ShaderGenerator::GeneratedMaterial genM =
        ShaderGenerator::GenerateMaterial(finalRenderParams, BlendMode::GetOpaque());
    if (!genM.ErrorMessage.empty())
    {
        std::cout << "Error generating final render mat: " << genM.ErrorMessage << "\n";
        char dummy;
        std::cin >> dummy;
        EndWorld();
        return;
    }
    finalRenderMat = genM.Mat;
    finalRenderParams.Texture2Ds["u_tex"].Texture = worldColor.GetTextureHandle();


    //Load world objects.
    GeoSets sets;
    XmlReader reader("Content/Old Ones/WorldGeoObjects.xml");
    reader.ReadDataStructure(sets);
    for (unsigned int i = 0; i < sets.Sets.size(); ++i)
    {
        std::cout << "Loading '" << sets.Sets[i].MeshFile << "'...\n";

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
    gameCam.PerspectiveInfo.zFar = 5000.0f;
    gameCam.Window = GetWindow();
}
void OldOnesWorld::OnWorldEnd(void)
{
    objs.clear();

    if (finalRenderMat != 0)
    {
        delete finalRenderMat;
    }
    if (worldRT != 0)
    {
        delete worldRT;
    }
    if (ppEffects != 0)
    {
        delete ppEffects;
    }

    worldColor.DeleteIfValid();
    worldDepth.DeleteIfValid();

    DrawingQuad::DestroyQuad();
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
    //Set up world rendering.
    Matrix4f viewM, projM;
    gameCam.GetViewTransform(viewM);
    gameCam.GetPerspectiveProjection(projM);
    RenderInfo info(GetTotalElapsedSeconds(), &gameCam, &viewM, &projM);

    //Draw world geometry.
    worldRT->EnableDrawingInto();
    RenderWorld(info);
    worldRT->DisableDrawingInto(windowSize.x, windowSize.y);

    //Render post-process effects.
    RenderObjHandle finalCol = ppEffects->Render(GetTotalElapsedSeconds(),
                                                 gameCam.PerspectiveInfo,
                                                 worldColor.GetTextureHandle(),
                                                 worldDepth.GetTextureHandle());

    //Render the final image.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ScreenClearer(true, true, false, Vector4f(1.0f, 0.0f, 1.0f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE, false, false).EnableState();
    glViewport(0, 0, windowSize.x, windowSize.y);
    Matrix4f identity;
    Camera finalRendCam;
    info = RenderInfo(GetTotalElapsedSeconds(), &finalRendCam, &identity, &identity);
    finalRenderParams.Texture2Ds["u_tex"].Texture = finalCol;
    DrawingQuad::GetInstance()->Render(info, finalRenderParams, *finalRenderMat);
}
void OldOnesWorld::RenderWorld(RenderInfo& info)
{
    RenderingState(RenderingState::C_NONE).EnableState();
    ScreenClearer(true, true, false, Vector4f(0.65f, 0.65f, 0.65f, 0.0f)).ClearScreen();
    glViewport(0, 0, renderSize.x, renderSize.y);

    for (unsigned int i = 0; i < objs.size(); ++i)
    {
        objs[i]->Render(info);
    }
    skybox->Render(info);
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

    renderSize = GetWorldRenderSize(windowSize);

    gameCam.PerspectiveInfo.Width = newWidth;
    gameCam.PerspectiveInfo.Height = newHeight;

    worldColor.ClearData(renderSize.x, renderSize.y);
    worldRT->UpdateSize();

    ppEffects->Resize(windowSize);
}