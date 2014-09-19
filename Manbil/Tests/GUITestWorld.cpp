#include "GUITestWorld.h"

#include <stdio.h>
#include <wchar.h>

#include "../Vertices.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"
#include "../Rendering/Curves/BezierCurve.h"

#include "../ScreenClearer.h"
#include "../RenderingState.h"

#include "../DebugAssist.h"


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
unsigned int textRendererID = FreeTypeHandler::ERROR_ID;

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

    return "";
}




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


    //Generate the curve.
    std::vector<CurveVertex> bezVerts;
    CurveVertex::GenerateVertices(bezVerts, 100);
    RenderObjHandle vbo;
    RenderDataHandler::CreateVertexBuffer(vbo, bezVerts.data(), bezVerts.size(), RenderDataHandler::UPDATE_ONCE_AND_DRAW);
    curveMesh.SetVertexIndexData(VertexIndexData(bezVerts.size(), vbo));


    //Generate the curve material.

    typedef std::shared_ptr<DataNode> DNP;

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

    ShaderGenerator::GeneratedMaterial genMat = ShaderGenerator::GenerateMaterial(curveParams, RenderingModes::RM_Opaque);
    if (!ReactToError(genMat.ErrorMessage.empty(), "Error generating curve material", genMat.ErrorMessage))
        return;
    curveMat = genMat.Mat;
    curveParams.FloatUniforms["u_startSlope"].SetValue(curveStartSlope);
    curveParams.FloatUniforms["u_endSlope"].SetValue(curveEndSlope);


    //Load the font.
    err = LoadFont(TextRender, "Content/Fonts/Candara.ttf", 25);
    if (!ReactToError(err.empty(), "Error loading 'Content/Fonts/Candara.ttf'", err))
        return;


    //Set up the GUI material.
    UniformDictionary guiElParamsCol;
    DNP lerpParam(new ParamNode(1, GUIMaterials::DynamicQuadDraw_TimeLerp, "timeLerpParam"));
    DNP lerpColor(new InterpolateNode(Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector4f(0.5f, 0.5f, 0.5f, 1.0f),
                                      lerpParam, InterpolateNode::IT_Linear, "lerpColor"));

    genMat = GUIMaterials::GenerateDynamicQuadDrawMaterial(guiElParamsCol, false,
                                                           Vector2f(1.0f, 1.0f),
                                                           lerpColor);
    if (!ReactToError(genMat.ErrorMessage.empty(), "Error generating color gui element material", genMat.ErrorMessage))
        return;
    guiMatColor = genMat.Mat;

    UniformDictionary guiElParamsGrey;
    genMat = GUIMaterials::GenerateDynamicQuadDrawMaterial(guiElParamsGrey, true, Vector2f(1.0f, 1.0f), lerpColor);
    if (!ReactToError(genMat.ErrorMessage.empty(), "Error generating greyscale gui element material", genMat.ErrorMessage))
        return;
    guiMatGrey = genMat.Mat;


    //Set up the GUI textures.

    guiTexData.Create(guiTexData.GetSamplingSettings(), false, PixelSizes::PS_32F);
    Array2D<Vector4f> guiTexCols(256, 128);
    guiTexCols.FillFunc([](Vector2u loc, Vector4f * outVal) { *outVal = Vector4f((float)loc.x / 128.0f, (float)loc.y / 128.0f, 1.0f, 1.0f); });
    guiTexData.SetColorData(guiTexCols);

    Array2D<Vector4b> whiteCol(1, 1, Vector4b(1.0f, 1.0f, 1.0f, 1.0f));
    guiBarTex.Create();
    guiBarTex.SetColorData(whiteCol);
    guiNubTex.Create();
    guiNubTex.SetColorData(whiteCol);


    //Set up the GUI elements.

    guiManager = GUIManager();

    unsigned int guiLabelSlot = TextRender->GetNumbSlots(textRendererID);
    if (!ReactToError(TextRender->CreateTextRenderSlots(textRendererID, 300, 64, false,
                                                        TextureSampleSettings2D(FT_LINEAR, FT_LINEAR,
                                                                                WT_CLAMP, WT_CLAMP)),
                      "Error creating text render slot for GUI label", TextRender->GetError()))
    {
        return;
    }
    GUILabel textBoxText(guiElParamsGrey, TextRender, TextRenderer::FontSlot(textRendererID, guiLabelSlot), guiMatGrey, 1.0f,
                         GUILabel::HO_LEFT, GUILabel::VO_CENTER);
    if (!ReactToError(textBoxText.SetText("change me"), "Error setting text box starting string", TextRender->GetError()))
        return;
    textBoxText.SetPosition(ToV2f(WindowSize) * 0.5f);
    textBoxText.SetScale(Vector2f(1.0f, 1.0f));
    GUITexture textBoxTex(guiElParamsCol, &guiBarTex, guiMatColor, false, 1.0f),
               cursorTex(guiElParamsCol, &guiBarTex, guiMatColor, false, 1.0f);
    cursorTex.Scale.x = 10.0f;
    textBoxTex.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    cursorTex.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(Vector4f(0.0f, 0.0f, 0.15f, 1.0f));
    GUITextBox* guiTextBox = new GUITextBox(textBoxTex, cursorTex, GUITexture(), textBoxText, 300.0f, 50.0f, true, guiElParamsCol, 1.0f);
    guiTextBox->SetColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
    guiTextBox->Box.SetColor(Vector4f(0.2f, 0.2f, 0.2f, 1.0f));
    GUIElementPtr guiTextBoxPtr(guiTextBox);

    GUITexture* guiTex = new GUITexture(guiElParamsCol, &guiTexData, guiMatColor, true, 9.0f);
    guiTex->IsButton = true;
    guiTex->OnClicked = [](GUITexture * clicked, Vector2f mouse, void* pData)
    {
        std::cout << "Clicked texture button. Screen pos: " <<
                     DebugAssist::ToString(mouse + clicked->GetCollisionCenter()) << "\n";
    };
    guiTex->SetPosition(ToV2f(WindowSize) * 0.5f);
    guiTex->SetScale(Vector2f(0.6f, 0.6f));
    GUIElementPtr guiTexPtr(guiTex);

    GUISlider * guiBar = new GUISlider(guiElParamsCol, guiElParamsCol, guiElParamsCol,
                                       &guiBarTex, &guiNubTex, guiMatColor, guiMatColor,
                                       Vector2f(200.0f, 10.0f), Vector2f(12.5f, 25.0f),
                                       false, false, 1.0f);
    guiBar->SetPosition(Vector2f(200.0f, 200.0f));
    guiBar->IsClickable = true;
    guiBar->Value = 0.5f;
    guiBar->Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
    guiBar->Bar.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
    guiBar->Nub.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
    GUIElementPtr guiBarPtr(guiBar);

    GUICheckbox* guiCheckbox = new GUICheckbox(guiElParamsCol,
                                               GUITexture(guiElParamsCol, &guiBarTex, guiMatColor),
                                               GUITexture(guiElParamsCol, &guiBarTex, guiMatColor),
                                               false, 1.0f);
    guiCheckbox->Box.SetColor(Vector4f(0.5f, 0.5f, 0.5f, 1.0f));
    guiCheckbox->Check.SetColor(Vector4f(0.2f, 0.2f, 0.58f, 1.0f));
    guiCheckbox->Check.ScaleBy(Vector2f(0.5f, 0.5f));
    guiCheckbox->SetScale(Vector2f(20.0f, 20.0f));
    GUIElementPtr guiCheckboxPtr(guiCheckbox);

    GUITexture guiSelectorBox(guiElParamsCol, &guiTexData, guiMatColor);
    std::vector<std::string> guiSelectorItems;
    guiSelectorItems.resize(3);
    guiSelectorItems[0] = "Index 0";
    guiSelectorItems[1] = "Index 1";
    guiSelectorItems[2] = "Index 2";
    GUISelectionBox* guiSelector = new GUISelectionBox(guiElParamsCol, TextRender, guiSelectorBox, textRendererID,
                                                       Vector2u(guiTexData.GetWidth(), 100), TextureSampleSettings2D(FT_NEAREST, WT_CLAMP),
                                                       guiMatGrey, GUILabel::HO_LEFT,
                                                       *guiTex, guiSelectorItems);
    guiSelector->ExtendAbove = true;
    if (!ReactToError(guiSelector->BoxElement.Mat != 0,
                      "Error generating GUI selection box",
                      TextRender->GetError()))
        return;
    guiSelector->SetPosition(Vector2f(80.0f, 80.0f));
    GUIElementPtr guiSelectorPtr(guiSelector);

    guiManager.GetFormattedRoot().AddObject(GUIFormatObject(GUIFormatObject::GUIElementType(guiTexPtr), 5.0f));
    guiManager.GetFormattedRoot().AddObject(GUIFormatObject(GUIFormatObject::GUIElementType(guiCheckboxPtr)));
    guiManager.GetFormattedRoot().AddObject(GUIFormatObject(20.0f));
    guiManager.GetFormattedRoot().AddObject(GUIFormatObject(GUIFormatObject::GUIElementType(guiTextBoxPtr), 5.0f));
    guiManager.GetFormattedRoot().AddObject(GUIFormatObject(GUIFormatObject::GUIElementType(guiBarPtr)));
    guiManager.GetFormattedRoot().AddObject(GUIFormatObject(GUIFormatObject::HorzBreakType(10.0f)));
    guiManager.GetFormattedRoot().AddObject(GUIFormatObject(GUIFormatObject::GUIElementType(guiSelectorPtr)));

    guiManager.GetFormattedRoot().SetPosition(ToV2f(WindowSize) * 0.5f);


    //Set up the back buffer.
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    GetWindow()->setSize(sf::Vector2u(WindowSize.x, WindowSize.y));
}
void GUITestWorld::DestroyMyStuff(bool destroyStatics)
{
    DeleteAndSetToNull(curveMat);

    DeleteAndSetToNull(guiMatColor);
    DeleteAndSetToNull(guiMatGrey);

    guiBarTex.DeleteIfValid();
    guiNubTex.DeleteIfValid();
    guiTexData.DeleteIfValid();

    if (destroyStatics) DestroyStaticSystems(false, true, true);
}


void GUITestWorld::UpdateWorld(float elapsed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        EndWorld();
    
    //Move the gui window.
    const float speed = 150.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        guiManager.GetFormattedRoot().MoveElement(Vector2f(-(speed * elapsed), 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        guiManager.GetFormattedRoot().MoveElement(Vector2f((int)(speed * elapsed), 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        guiManager.GetFormattedRoot().MoveElement(Vector2f(0.0f, (speed * elapsed)));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        guiManager.GetFormattedRoot().MoveElement(Vector2f(0.0f, -(speed * elapsed)));

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
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    ScreenClearer(true, true, false, Vector4f(0.1f, 0.1f, 0.1f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE, RenderingState::BE_SOURCE_ALPHA, RenderingState::BE_ONE_MINUS_SOURCE_ALPHA,
                   false, false, RenderingState::AT_GREATER, 0.0f).EnableState();

    //Set up the "render info" struct.
    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
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