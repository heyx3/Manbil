#include "GUITestWorld.h"

#include <stdio.h>
#include <wchar.h>

#include "../Rendering/Basic Rendering/Vertices.h"
#include "../Rendering/Data Nodes/ShaderGenerator.h"
#include "../Rendering/Data Nodes/DataNodes.hpp"

#include "../Rendering/Basic Rendering/ScreenClearer.h"
#include "../Rendering/Basic Rendering/RenderingState.h"
#include "../Rendering/GUI/GUI Elements/GUIPanel.h"
#include "../Editor/EditorPanel.h"
#include "../Editor/EditorObjects.h"

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
}


//Used for testing EditorCollection.
struct CollectionElement : public IEditable
{
public:
    unsigned int Int = 4;
    std::string String = "Test";
    virtual std::string BuildEditorElements(std::vector<EditorObjectPtr>& outElements,
                                            EditorMaterialSet& materialSet) override
    {
        EditorObjectPtr slideBar(new SlidingBarInt(0, 100, Vector2f(),
                                                   EditorObject::DescriptionData(),
                                                   [](GUISlider* slider, int newVal, void* pData)
                                                   {
                                                       std::cout << newVal << "\n";
                                                   },
                                                   Mathf::LerpComponent(0.0f, 0.0f, Int)));
        outElements.push_back(slideBar);

        EditorObjectPtr textBox(new TextBoxString(String, 600.0f, Vector2f(),
                                                  EditorObject::DescriptionData(),
                                                  [](GUITextBox* slider, std::string newVal, void* pData)
                                                  {
                                                      std::cout << newVal << "\n";
                                                  }));
        outElements.push_back(textBox);

        return "";
    }
};

void GUITestWorld::InitializeWorld(void)
{
    std::string err;


    SFMLOpenGLWorld::InitializeWorld();

    //Initialize systems.
    DrawingQuad::InitializeQuad();
    if (!ReactToError(!FreeTypeHandler::Instance.HasError(),
                      "Error initializing FreeTypeHandler",
                      FreeTypeHandler::Instance.GetError()))
    {
        return;
    }
    err = TextRenderer::InitializeSystem();
    if (!ReactToError(err.empty(), "Error initializing text renderer", err))
    {
        return;
    }
    TextRender = new TextRenderer();


    //Set up editor content.
    editorMaterials = new EditorMaterialSet(*TextRender);
    err = EditorMaterialSet::GenerateDefaultInstance(*editorMaterials);
    if (!ReactToError(err.empty(), "Error loading default editor materials", err))
    {
        delete editorMaterials;
        editorMaterials = 0;
        return;
    }

    //Build the GUI elements.
    //There are a couple different branches here, in order to show various aspects of the editor system.
    if (false)
    {
        //Use an empty panel.
        guiManager = GUIManager(GUIElementPtr(new GUIPanel(GUITexture())));
    }
    else if (false)
    {
        //Build out some individual editor elements that just print out their values when changed.

        EditorPanel* editor = new EditorPanel(*editorMaterials, 00.0f, 00.0f);
        
        //Checkbox.
        EditorObjectPtr checkbox(new CheckboxValue(EditorObject::DescriptionData("Check this box:"),
                                                   Vector2f(), false));
        editor->AddObject(checkbox);

        //Text box for an unsigned int value.
        EditorObjectPtr textBoxU(new TextBoxUInt(56, 200.0f, Vector2f(),
                                                 EditorObject::DescriptionData("Type a uint!"),
                                                 [](GUITextBox* textBox, unsigned int newVal, void* pDat)
                                                 {
                                                     std::cout << "UInt: " << newVal << "\n";
                                                 }));
        editor->AddObject(textBoxU);

        //Button.
        EditorObjectPtr button(new EditorButton("Push me", Vector2f(200.0f, 50.0f), 0, Vector2f(),
                                                EditorObject::DescriptionData("<-- Push this!",
                                                                              false, 30.0f),
                                                [](GUITexture* clicked, Vector2f mousePos, void* pDat)
                                                {
                                                    std::cout << "Clicked button\n";
                                                }));
        editor->AddObject(button);

        //Label.
        editor->AddObject(EditorObjectPtr(new EditorLabel("I'm just a label.", 800)));

        //Collapsible region with some GUI elements inside.
        MTexture2D* testTex = &editorMaterials->DeleteFromCollectionTex;
        GUIElementPtr innerEl(new GUITexture(editorMaterials->GetStaticMatParams(testTex), testTex,
                                             editorMaterials->GetStaticMaterial(testTex)));
        EditorObjectPtr collapseRegion(new EditorCollapsibleBranch(innerEl, 20.0f,
                                                                   "This is collapsible"));
        editor->AddObject(collapseRegion);
        
        //A collection of a data structure, defined above.
        editor->AddObject(EditorObjectPtr(new EditorCollection<CollectionElement>()));

        //Stick the editor element into the GUI root manager.
        guiManager = GUIManager(GUIElementPtr(editor));
    }
    else if (true)
    {
        //Generate an editor panel for editing the values in ColorEditor instance called "colEd".

        EditorPanel* editor = new EditorPanel(*editorMaterials, 00.0f, 00.0f);

        std::vector<EditorObjectPtr> ptrs;
        colEd.Color = Vector4f(0.5f, 0.25f, 1.0f, 0.5f);
        std::string err = colEd.BuildEditorElements(ptrs, *editorMaterials);
        if (!ReactToError(err.empty(), "Error building color editor elements", err))
        {
            return;
        }

        err = editor->AddObjects(ptrs);
        if (!ReactToError(err.empty(), "Error adding editor elements", err))
        {
            return;
        }

        //Stick the editor element into the GUI root manager.
        guiManager = GUIManager(GUIElementPtr(editor));
    }
    else
    {
        //This is just here to make sure that one of the above branches is actually used.
        assert(false);
    }

    //Set up the window.
    WindowSize = Vector2i(500, 500);
    Vector2f dims = guiManager.RootElement->GetBounds().GetDimensions();
    guiManager.RootElement->SetBounds(Box2D(dims.ComponentProduct(Vector2f(0.5f, -0.5f)), dims));
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    GetWindow()->setSize(sf::Vector2u(WindowSize.x, WindowSize.y));
    GetWindow()->setPosition(sf::Vector2i(0, 0));
    
}
void GUITestWorld::OnWorldEnd(void)
{
    delete editorMaterials;
    delete TextRender;

    DrawingQuad::DestroyQuad();
    TextRenderer::DestroySystem();
}


void GUITestWorld::UpdateWorld(float elapsed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
    }
    
    //Move the gui window with WASD.
    const float speed = 100.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        guiManager.RootElement->MoveElement(Vector2f(-(speed * elapsed), 0.0f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        guiManager.RootElement->MoveElement(Vector2f((speed * elapsed), 0.0f));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        guiManager.RootElement->MoveElement(Vector2f(0.0f, -(speed * elapsed)));
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        guiManager.RootElement->MoveElement(Vector2f(0.0f, (speed * elapsed)));
    }

    sf::Vector2i mPos = sf::Mouse::getPosition();
    sf::Vector2i mPosFinal = mPos - GetWindow()->getPosition() - sf::Vector2i(5, 30);
    mPosFinal.y -= WindowSize.y;

    guiManager.Update(elapsed, Vector2i(mPosFinal.x, mPosFinal.y),
                      sf::Mouse::isButtonPressed(sf::Mouse::Left));
}
void GUITestWorld::RenderOpenGL(float elapsed)
{
    //Set up the rendering state.
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    ScreenClearer(true, true, false, Vector4f(0.5f, 0.2f, 0.2f, 0.0f)).ClearScreen();
    //TODO: Set culling mode once I can test this world.
    RenderingState(RenderingState::C_NONE, false, true).EnableState();

    //Set up the "render info" struct.
    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, -1.0f, 0.0f));
    cam.MinOrthoBounds = Vector3f(0.0f, 0.0f, -10.0f);
    cam.MaxOrthoBounds = Vector3f((float)WindowSize.x, (float)WindowSize.y, 10.0f);
    cam.PerspectiveInfo.Width = WindowSize.x;
    cam.PerspectiveInfo.Height = WindowSize.y;
    Matrix4f viewM, projM;
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);

    //Render the GUI.
    RenderInfo info(GetTotalElapsedSeconds(), &cam, &viewM, &projM);
    guiManager.Render(elapsed, info);
}