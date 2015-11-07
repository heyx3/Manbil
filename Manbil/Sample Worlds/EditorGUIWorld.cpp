#include "EditorGUIWorld.h"

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




//First define a data structure that will be "edited".
struct MyData : public IEditable
{
public:

    int Int = 4;
    std::string String = "Hello World";


    //This function builds the set of editor widgets to use when editing this instance.
    virtual std::string BuildEditorElements(std::vector<EditorObjectPtr>& outElements,
                                            EditorMaterialSet& materialSet) override
    {
        //Many different editor widgets are defined in "EditorObjects.h".

        //Use a slider for the Int field, ranging from 0 to 100.
        //The slider is set up to pass a pointer to our "Int" field into the callback so that we can set it.
        SlidingBarInt<int*>* intSliderPtr =
            new SlidingBarInt<int*>(0, 100, Vector2f(), EditorObject::DescriptionData("Int"),
                                    [](GUISlider* slider, int nextVal, int* pData)
                                    {
                                        //*pData = nextVal;
                                        std::cout << "Int changed to " << nextVal << "\n";
                                    },
                                    Mathf::LerpComponent(0.0f, 100.0f, Int), 1.0f,
                                    &Int);
        outElements.push_back(EditorObjectPtr(intSliderPtr));

        //Use a text box for the String field.
        //Like the slider, it is set up to pass a pointer to our "String" field into the callback.
        TextBoxString<std::string*>* stringBoxPtr =
            new TextBoxString<std::string*>(String, 600.0f, Vector2f(),
                                            EditorObject::DescriptionData("String"),
                                            [](GUITextBox* box, std::string newVal, std::string* pData)
                                            {
                                                //*pData = newVal;
                                                std::cout << "String changed to '" << newVal << "'\n";
                                            },
                                            &String);
        outElements.push_back(EditorObjectPtr(stringBoxPtr));

        //This function should return an error message if anything went wrong.
        return "";
    }
};




EditorGUIWorld::EditorGUIWorld(void)
    : SFMLOpenGLWorld(800, 600, sf::ContextSettings(24, 0, 0, 4, 1))
{
}


bool EditorGUIWorld::Assert(bool isOK, std::string errIntro, std::string errMsg)
{
    if (!isOK)
    {
        std::cout << errIntro << ": " << errMsg << "\n";
        char dummy;
        std::cin >> dummy;
    }
    return isOK;
}
void EditorGUIWorld::OnInitializeError(std::string errMsg)
{
    SFMLOpenGLWorld::OnInitializeError(errMsg);
    char dummy;
    std::cin >> dummy;
    EndWorld();
}


void EditorGUIWorld::OnWindowResized(unsigned int newW, unsigned int newH)
{
    WindowSize.x = newW;
    WindowSize.y = newH;
}


bool EditorGUIWorld::InitializeEditorStuff(void)
{
    //First, generate the content that the editor panel will use to render itself.
    editorMaterials = new EditorMaterialSet(*TextRender);

    //You can supply your own content, but there is also a set of "default" content for the lazy.
    //This content is stored in "Dependencies/Include In Build/Universal/Content/Default Editor Content/".
    std::string err = EditorMaterialSet::GenerateDefaultInstance(*editorMaterials);
    if (!Assert(err.empty(), "Error loading default editor materials", err))
    {
        delete editorMaterials;
        editorMaterials = 0;
        return false;
    }


    //Next, generate an editor panel for the "MyData" data structure defined above.
    //The "EditorPanel" class is a child of the "GUIElement" class.
    
    EditorPanel* editor = new EditorPanel(*editorMaterials, 0.0f, 0.0f);
    
    MyData dataToEdit;
    std::vector<EditorObjectPtr> editorWidgets;
    err = dataToEdit.BuildEditorElements(editorWidgets, *editorMaterials);
    if (!Assert(err.empty(), "Error building editor elements", err))
    {
        delete editor;
        return false;
    }

    err = editor->AddObjects(editorWidgets);
    if (!Assert(err.empty(), "Error adding editor elements to editor panel", err))
    {
        delete editor;
        return false;
    }

    //Give the editor panel to the GUI manager.
    guiManager = GUIManager(GUIElementPtr(editor));

    return true;
}
void EditorGUIWorld::InitializeWorld(void)
{
    std::string err;

    SFMLOpenGLWorld::InitializeWorld();

    //If there was an error initializing the window/OpenGL,
    //    don't bother with the rest of the initialization.
    if (IsGameOver())
    {
        return;
    }

    //Initialize systems.
    DrawingQuad::InitializeQuad();
    err = TextRenderer::InitializeSystem();
    if (!Assert(err.empty(), "Error initializing text renderer", err))
    {
        return;
    }
    TextRender = new TextRenderer();


    //Set up the editor.
    if (!InitializeEditorStuff())
    {
        return;
    }


    //Now that the editor is set up, resize the window to fit the editor panel.
    Vector2f size = guiManager.RootElement->GetBounds().GetDimensions();
    WindowSize = ToV2i(size);
    glViewport(0, 0, WindowSize.x, WindowSize.y);
    GetWindow()->setSize(sf::Vector2u(WindowSize.x, WindowSize.y));

    //Move the panel to be centered in the window.
    //The panel's center starts at the origin, in the bottom-left of the window.
    guiManager.RootElement->MoveElement(size.FlipY() * 0.5f);
}

void EditorGUIWorld::OnWorldEnd(void)
{
    //First delete the root GUI element; this must be done before cleaning up the text-rendering system.
    guiManager.SetRoot(0);

    delete editorMaterials;
    delete TextRender;

    DrawingQuad::DestroyQuad();
    TextRenderer::DestroySystem();
}

void EditorGUIWorld::UpdateWorld(float elapsed)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
    {
        EndWorld();
    }

    //Get the mouse position offset relative to the window screen.
    sf::Vector2i mPos = sf::Mouse::getPosition();
    sf::Vector2i mPosFinal = mPos - GetWindow()->getPosition() - sf::Vector2i(5, 30);
    mPosFinal.y -= WindowSize.y;

    //Update the GUI elements.
    guiManager.Update(elapsed, Vector2i(mPosFinal.x, mPosFinal.y),
                      sf::Mouse::isButtonPressed(sf::Mouse::Left));
}

void EditorGUIWorld::RenderOpenGL(float elapsed)
{
    //Set up rendering state.
    //TODO: Try enabling culling once I can run this world successfully.
    ScreenClearer(true, true, false, Vector4f(0.5f, 0.2f, 0.2f, 0.0f)).ClearScreen();
    RenderingState(RenderingState::C_NONE, false, true).EnableState();
    glViewport(0, 0, WindowSize.x, WindowSize.y);

    //Set up the rendering info.
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