#include "GUITestWorld.h"

#include <stdio.h>
#include <wchar.h>

#include "../Vertices.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Curves/BezierCurve.h"

#include "../Rendering/GUI/GUI Elements/GUILabel.h"
#include "../Rendering/GUI/GUI Elements/GUITexture.h"
#include "../Rendering/GUI/GUI Elements/GUISlider.h"
#include "../Rendering/GUI/GUI Elements/GUISelectionBox.h"

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


Vector2i GUITestWorld::WindowSize = Vector2i(1024, 1024);
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
    if (!rendr->CreateTextRenderSlots(textRendererID, textSize.x, textSize.y, false,
                                      TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
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


    //Create the quad rendering material.

    typedef std::shared_ptr<DataNode> DNP;
    DataNode::ClearMaterialData();
    DataNode::VertexIns = DrawingQuad::GetAttributeData();

    DNP objPosToScreen = SpaceConverterNode::ObjPosToScreenPos(VertexInputNode::GetInstance(), "objPosToScreen");
    DataNode::MaterialOuts.VertexPosOutput = DataLine(objPosToScreen, 1);

    DataNode::MaterialOuts.VertexOutputs.insert(DataNode::MaterialOuts.VertexOutputs.end(),
                                                ShaderOutput("vOut_UV",
                                                             DataLine(VertexInputNode::GetInstance(), 1)));
    
    DNP textSamplePtr(new TextureSample2DNode(FragmentInputNode::GetInstance(), textSamplerName, "textSample"));
    DataLine textSampleRed(textSamplePtr, TextureSample2DNode::GetOutputIndex(CO_Red));
    DNP textSampleRGB1(new CombineVectorNode(textSampleRed, textSampleRed, textSampleRed, 1.0f, "textSampleRGB"));
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_FinalColor", textSampleRGB1));

    ShaderGenerator::GeneratedMaterial genMat = ShaderGenerator::GenerateMaterial(quadParams, RenderingModes::RM_Opaque);
    if (!ReactToError(genMat.ErrorMessage.empty(), "Error generating quad material", genMat.ErrorMessage))
        return;
    quadMat = genMat.Mat;


    //Generate the curve.
    std::vector<CurveVertex> bezVerts;
    CurveVertex::GenerateVertices(bezVerts, 100);
    RenderObjHandle vbo;
    RenderDataHandler::CreateVertexBuffer(vbo, bezVerts.data(), bezVerts.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    curveMesh.SetVertexIndexData(VertexIndexData(bezVerts.size(), vbo));

    //Generate the curve material.

    DataNode::ClearMaterialData();
    DataNode::VertexIns = CurveVertex::GetAttributeData();

    DNP startSlopeParam(new ParamNode(3, "u_startSlope", "startSlopeParam")),
        endSlopeParam(new ParamNode(3, "u_endSlope", "endSlopeParam"));
    DNP curvePositioning(new BezierCurve(Vector3f(-0.5f, -0.5f, 0.0f), Vector3f(0.5f, 0.5f, 0.0f),
                                         startSlopeParam, endSlopeParam, Vector3f(0.0f, 0.0f, 1.0f),
                                         0.005f, 0, "myCurve"));
    DNP curveOutPos(new CombineVectorNode(curvePositioning, 1.0f, "curveOutPos"));
    DataNode::MaterialOuts.VertexPosOutput = curveOutPos;
    
    DataNode::MaterialOuts.FragmentOutputs.insert(DataNode::MaterialOuts.FragmentOutputs.end(),
                                                  ShaderOutput("fOut_curveCol", Vector4f(1.0f, 1.0f, 1.0f, 1.0f)));

    genMat = ShaderGenerator::GenerateMaterial(curveParams, RenderingModes::RM_Opaque);
    if (!ReactToError(genMat.ErrorMessage.empty(), "Error generating curve material", genMat.ErrorMessage))
        return;
    curveMat = genMat.Mat;
    curveParams.FloatUniforms["u_startSlope"].SetValue(curveStartSlope);
    curveParams.FloatUniforms["u_endSlope"].SetValue(curveEndSlope);


    //Load the font.
    err = LoadFont(TextRender, "Content/Fonts/Candara.ttf", 25);
    if (!ReactToError(err.empty(), "Error loading 'Content/Fonts/Candara.ttf'", err))
        return;

    //Render a string.
    err = RenderText(TextRender, "Hello, World!!");
    if (!ReactToError(err.empty(), "Error rendering the text: ", err))
        return;
    quadParams.Texture2DUniforms[textSamplerName].Texture = TextRender->GetRenderedString(textRendererID)->GetTextureHandle();


    //Size the quad to be the size of the string.
    Vector2i tScale = TextRender->GetSlotRenderSize(TextRenderer::FontSlot(textRendererID, 0));
    quad->SetSize(Vector2f((float)tScale.x, (float)tScale.y));
    quad->SetPos(Vector2f(0.0f, 0.0f));
    quad->SetOrigin(Vector2f());


    //Set up the GUI material.
    UniformDictionary guiElParams;
    genMat = GUIMaterials::GenerateStaticQuadDrawMaterial(guiElParams);
    if (!ReactToError(genMat.ErrorMessage.empty(), "Error generating gui element material", genMat.ErrorMessage))
        return;
    guiMat = genMat.Mat;


    //Set up the GUI elements.
    guiManager.GetRoot().SetPosition(Vector2i());
    guiManager.GetRoot().SetScale(Vector2f(500.0f, 500.0f));

    unsigned int guiLabelSlot = TextRender->GetNumbSlots(textRendererID);
    if (!ReactToError(TextRender->CreateTextRenderSlots(textRendererID, 512, 64, false,
                                                        TextureSampleSettings2D(FT_LINEAR, FT_LINEAR,
                                                                                WT_CLAMP, WT_CLAMP)),
                      "Error creating text render slot for GUI label", TextRender->GetError()))
    {
        return;
    }
    if (!ReactToError(TextRender->RenderString(TextRenderer::FontSlot(textRendererID, 1), "TestGUI"), "Error rendering gui string", TextRender->GetError()))
    {
        return;
    }
    guiLabel = GUIElement::Ptr(new GUILabel(TextRender, TextRenderer::FontSlot(textRendererID, guiLabelSlot), guiMat));
    guiLabel->Params = guiElParams;
    guiManager.GetRoot().Elements.insert(guiManager.GetRoot().Elements.end(), guiLabel);

    guiTexData.Create();
    Array2D<Vector4f> guiTexCols(128, 128);
    guiTexCols.FillFunc([](Vector2u loc, Vector4f * outVal) { *outVal = Vector4f((float)loc.x / 128.0f, (float)loc.y / 128.0f, 1.0f, 1.0f); });
    guiTexData.SetColorData(guiTexCols);
    guiTex = GUIElement::Ptr(new GUITexture(&guiTexData, guiMat, true, 1.0f));
    guiTex->Params = guiElParams;
    guiManager.GetRoot().Elements.insert(guiManager.GetRoot().Elements.end(), guiTex);


    //Set up the back buffer.
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    GetWindow()->setSize(sf::Vector2u(WindowSize.x, WindowSize.y));
}
void GUITestWorld::DestroyMyStuff(bool destroyStatics)
{
    DeleteAndSetToNull(quad);
    DeleteAndSetToNull(quadMat);
    DeleteAndSetToNull(curveMat);

    if (destroyStatics) DestroyStaticSystems(false, true, true);
}


void GUITestWorld::UpdateWorld(float elapsed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        EndWorld();

    const float speed = 150.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        guiManager.GetRoot().MoveElement(Vector2i(-(int)(speed * elapsed), 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        guiManager.GetRoot().MoveElement(Vector2i((int)(speed * elapsed), 0));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        guiManager.GetRoot().MoveElement(Vector2i(0.0f, (int)(speed * elapsed)));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        guiManager.GetRoot().MoveElement(Vector2i(0.0f, -(int)(speed * elapsed)));

    std::cout << "X: " << guiManager.GetRoot().GetCollisionCenter().x << "; Y: " << guiManager.GetRoot().GetCollisionCenter().y << "\n";

    sf::Vector2i mPos = sf::Mouse::getPosition();
    sf::Vector2i mPosFinal = mPos - GetWindow()->getPosition() - sf::Vector2i(5, 30);
    mPosFinal.y = WindowSize.y - mPosFinal.y;

    guiManager.Update(elapsed, Vector2i(mPosFinal.x, mPosFinal.y), sf::Mouse::isButtonPressed(sf::Mouse::Left));

    Vector3f slopePosVal(BasicMath::Remap(0.0f, 1.0f, -1.0f, 1.0f, (float)mPosFinal.x / (float)WindowSize.x),
                         BasicMath::Remap(0.0f, 1.0f, -1.0f, 1.0f, (float)mPosFinal.y / (float)WindowSize.y),
                         0.0f);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        curveStartSlope = slopePosVal;
        curveParams.FloatUniforms["u_startSlope"].SetValue(curveStartSlope);
    }
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
    {
        curveEndSlope = slopePosVal;
        curveParams.FloatUniforms["u_endSlope"].SetValue(curveEndSlope);
    }
}
void GUITestWorld::RenderOpenGL(float elapsed)
{
    //Prepare the back-buffer to be rendered into.
    ScreenClearer().ClearScreen();
    RenderingState(RenderingState::C_NONE, RenderingState::BE_SOURCE_ALPHA, RenderingState::BE_ONE_MINUS_SOURCE_ALPHA,
                   false, false).EnableState();

    //Set up the "render info" struct.
    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
    cam.MinOrthoBounds = Vector3f(0.0f, 0.0f, -10.0f);
    cam.MaxOrthoBounds = Vector3f((float)WindowSize.x, (float)WindowSize.y, 10.0f);
    cam.Info.Width = WindowSize.x;
    cam.Info.Height = WindowSize.y;
    Matrix4f worldM, viewM, projM;
    TransformObject trns;
    trns.GetWorldTransform(worldM);
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);
    RenderInfo info(this, &cam, &trns, &worldM, &viewM, &projM);

    //Render the quad.
    Vector2f oldQuadPos = quad->GetPos();
    Vector2i textSize = TextRender->GetSlotBoundingSize(TextRenderer::FontSlot(textRendererID)),
             textRenderSize = TextRender->GetSlotRenderSize(TextRenderer::FontSlot(textRendererID));
    Vector2f delta = (ToV2f(textSize) - ToV2f(textRenderSize)) * 0.5f;
    quad->IncrementPos(delta);
    if (!ReactToError(quad->Render(info, quadParams, *quadMat), "Error rendering quad", quadMat->GetErrorMsg()))
        return;
    quad->IncrementPos(-delta);

    //Render the curve.
    std::vector<const Mesh*> toRender;
    toRender.insert(toRender.end(), &curveMesh);
    if (!ReactToError(curveMat->Render(info, toRender, curveParams), "Error rendering curve", curveMat->GetErrorMsg()))
        return;

    //Render the GUI.
    std::string err = guiManager.Render(elapsed, info);
    if (!ReactToError(err.empty(), "Error rendering GUI", err))
        return;
}