#include "GUITestWorld.h"

#include <stdio.h>
#include <wchar.h>

#include "../Vertices.h"
#include "../Rendering/Materials/Data Nodes/ShaderGenerator.h"
#include "../Rendering/Materials/Data Nodes/DataNodeIncludes.h"

#include "../ScreenClearer.h"
#include "../RenderingState.h"
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


Vector2i GUITestWorld::WindowSize = Vector2i();


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
    virtual std::string BuildEditorElements(std::vector<EditorObjectPtr> & outElements,
                                            EditorMaterialSet & materialSet) override
    {
        outElements.insert(outElements.end(),
                           EditorObjectPtr(new SlidingBarUInt(0, 100, Vector2f(), EditorObject::DescriptionData(),
                                                              [](GUISlider* slider, unsigned int newVal, void* pData)
                                                                 { std::cout << newVal << "\n"; },
                                                              BasicMath::LerpComponent(0.0f, 100.0f, Int))));
        outElements.insert(outElements.end(),
                           EditorObjectPtr(new TextBoxString(String, 600.0f,
                                                             Vector2f(), EditorObject::DescriptionData(),
                                                             [](GUITextBox* textBox, std::string newVal, void* pData)
                                                             { std::cout << newVal << "\n"; })));

        return "";
    }
};

void GUITestWorld::InitializeWorld(void)
{
    std::string err;


    SFMLOpenGLWorld::InitializeWorld();

    //Initialize static stuff.
    err = InitializeStaticSystems(false, true, true);
    if (!ReactToError(err.empty(), "Error initializing static systems", err))
        return;


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
    if (false)
    {
        EditorPanel* editor = new EditorPanel(*editorMaterials, 00.0f, 00.0f);
        /*
        editor->AddObject(EditorObjectPtr(new CheckboxValue(EditorObject::DescriptionData("Check this shit"), Vector2f(), false)));
        editor->AddObject(EditorObjectPtr(new TextBoxUInt(56, Vector2u(200, 50), Vector2f(),
                                                          EditorObject::DescriptionData("Type a uint!"),
                                                          [](GUITextBox* textBox, unsigned int newVal, void* pData)
                                                            { std::cout << newVal << "\n"; })));
        editor->AddObject(EditorObjectPtr(new EditorButton("Push me", Vector2f(200.0f, 50.0f), 0, Vector2f(),
                                                           EditorObject::DescriptionData("<-- Push this shit", false, 30.0f),
                                                           [](GUITexture* clicked, Vector2f mp, void* pDat)
                                                            { std::cout << "Clicked\n"; })));
        editor->AddObject(EditorObjectPtr(new EditorLabel("I'm just a label.", 800)));

        MTexture2D* testTex = &editorMaterials->DeleteFromCollectionTex;
        GUIElementPtr innerEl(new GUITexture(editorMaterials->GetStaticMatParams(testTex), testTex,
                                             editorMaterials->GetStaticMaterial(testTex)));
        editor->AddObject(EditorObjectPtr(new EditorCollapsibleBranch(innerEl, 20.0f, "This is collapsible")));
        */
        editor->AddObject(EditorObjectPtr(new EditorCollection<CollectionElement>()));

        guiManager = GUIManager(GUIElementPtr(editor));
    }
    else if (false)
    {
        EditorPanel* editor = new EditorPanel(*editorMaterials, 00.0f, 00.0f);
        std::vector<EditorObjectPtr> ptrs;
        colEd.Color = Vector4f(0.5f, 0.25f, 1.0f, 0.5f);

        std::string err = colEd.BuildEditorElements(ptrs, *editorMaterials);
        if (!ReactToError(err.empty(), "Error building color editor elements", err))
            return;

        for (unsigned int i = 0; i < ptrs.size(); ++i)
        {
            err = editor->AddObject(ptrs[i]);
            if (!ReactToError(err.empty(), "Error adding ptr element #" + std::to_string(i + 1), err))
                return;
        }

        guiManager = GUIManager(GUIElementPtr(editor));
    }
    else if (true)
    {
        //Two sample textures.
        MTexture2D *tex1 = &editorMaterials->SelectionBoxBackgroundTex,
                   *tex2 = &editorMaterials->SelectionBoxBoxTex,
                   *tex3 = &editorMaterials->SliderBarTex;
        GUITexture guiTex1(editorMaterials->GetAnimatedMatParams(tex1), tex1,
                           editorMaterials->GetAnimatedMaterial(tex1), true,
                           editorMaterials->AnimateSpeed),
                   guiTex2(editorMaterials->GetAnimatedMatParams(tex2), tex2,
                           editorMaterials->GetAnimatedMaterial(tex2), false,
                           editorMaterials->AnimateSpeed),
                   guiTex3(editorMaterials->GetStaticMatParams(tex3), tex3,
                           editorMaterials->GetStaticMaterial(tex3), false,
                           editorMaterials->AnimateSpeed);
        
        guiTex3.SetBounds(guiTex2.GetBounds());
        guiTex3.ScaleBy(Vector2f(0.95f, 0.95f));
        guiTex3.SetColor(Vector4f(0.3f, 0.3f, 0.9f, 0.3f));

        std::vector<std::string> items;
        items.insert(items.end(), "First");
        items.insert(items.end(), "Second");
        items.insert(items.end(), "Third");
        items.insert(items.end(), "Fourth");
        GUISelectionBox* selector = new GUISelectionBox(err, &editorMaterials->TextRender, guiTex2, guiTex3,
                                                        guiTex1, true, Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
                                                        editorMaterials->FontID, editorMaterials->StaticMatText,
                                                        editorMaterials->StaticMatTextParams, true, FT_LINEAR,
                                                        items, editorMaterials->TextScale, 0.0f, 0, 0, 0, 0,
                                                        editorMaterials->AnimateSpeed);
        selector->SetExtendsAbove(false);

        selector->SetDrawEmptyItems(false);
        selector->SetItem(1, "");
        selector->SetItem(0, "");
        selector->SetItem(3, "");

        selector->SetItem(1, "Haha");

        /*
        TextRenderer::FontSlot slot(editorMaterials->FontID,
                                    editorMaterials->TextRender.GetNumbSlots(editorMaterials->FontID));
        if (!editorMaterials->TextRender.CreateTextRenderSlots(slot.FontID, 512, 512, true, TextureSampleSettings2D(FT_LINEAR, WT_CLAMP)))
        {
            EndWorld();
            return;
        }
        if (!editorMaterials->TextRender.RenderString(slot, "First"))
        {
            EndWorld();
            return;
        }
        GUILabel* lbl = new GUILabel(editorMaterials->StaticMatTextParams, &editorMaterials->TextRender,
                                     slot, editorMaterials->StaticMatText, 1.0f, GUILabel::HO_LEFT, GUILabel::VO_CENTER);

        */


        GUIElementPtr guiPtr(selector);
        //GUIElementPtr guiPtr(lbl);
        guiPtr->OnUpdate = [](GUIElement* thisEl, Vector2f relativeMouse, void* pData)
        {
            thisEl->SetScale(thisEl->GetScale() * 1.002f);
            //std::cout << DebugAssist::ToString(relativeMouse) << "\n";
        };

        //Create the GUIManager.
        guiManager = GUIManager(guiPtr);
    }
    else assert(false);

    //Set up the window.
    Vector2f dims = guiManager.RootElement->GetBounds().GetDimensions();
    //WindowSize = Vector2i((int)dims.x, (int)dims.y);
    WindowSize = Vector2i(500, 500);
    //guiManager.RootElement->SetPosition(dims * 0.5f);
    guiManager.RootElement->SetBounds(Box2D(dims.ComponentProduct(Vector2f(0.5f, -0.5f)), dims));
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    GetWindow()->setSize(sf::Vector2u(WindowSize.x, WindowSize.y));
}
void GUITestWorld::DestroyMyStuff(bool destroyStatics)
{
    DeleteAndSetToNull(editorMaterials);

    if (destroyStatics) DestroyStaticSystems(false, true, true);
}


void GUITestWorld::UpdateWorld(float elapsed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        EndWorld();
    
    //Move the gui window.
    const float speed = 100.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        guiManager.RootElement->MoveElement(Vector2f(-(speed * elapsed), 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        guiManager.RootElement->MoveElement(Vector2f((speed * elapsed), 0.0f));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        guiManager.RootElement->MoveElement(Vector2f(0.0f, -(speed * elapsed)));
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        guiManager.RootElement->MoveElement(Vector2f(0.0f, (speed * elapsed)));

    sf::Vector2i mPos = sf::Mouse::getPosition();
    sf::Vector2i mPosFinal = mPos - GetWindow()->getPosition() - sf::Vector2i(5, 30);
    mPosFinal.y -= WindowSize.y;

    guiManager.Update(elapsed, Vector2i(mPosFinal.x, mPosFinal.y), sf::Mouse::isButtonPressed(sf::Mouse::Left));
    //guiManager.RootElement->SetPosition(Vector2f(WindowSize.x * 0.5f, WindowSize.y * 0.5f));
}
void GUITestWorld::RenderOpenGL(float elapsed)
{
    //Prepare the back-buffer to be rendered into.
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    ScreenClearer(true, true, false, Vector4f(0.5f, 0.2f, 0.2f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE, RenderingState::BE_SOURCE_ALPHA, RenderingState::BE_ONE_MINUS_SOURCE_ALPHA,
                   true, true, RenderingState::AT_GREATER, 0.0f).EnableState();

    //Set up the "render info" struct.
    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, -1.0f, 0.0f));
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

    //Render the GUI.
    std::string err = guiManager.Render(elapsed, info);
    if (!ReactToError(err.empty(), "Error rendering GUI", err))
        return;
}