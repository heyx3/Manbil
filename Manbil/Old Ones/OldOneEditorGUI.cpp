#include "OldOneEditorGUI.h"

#include "../Rendering/Rendering.hpp"

#include "../Editor/EditorPanel.h"


OldOneEditorGUI::OldOneEditorGUI(std::string& outErr)
    : textRender(), materials(textRender)
{
    //Generate materials for the editor panel.
    //Just use the default materials.
    outErr = EditorMaterialSet::GenerateDefaultInstance(materials);
    if (!outErr.empty())
    {
        outErr = "Error generating editor panel's material set: " + outErr;
        return;
    }

    //Generate editor GUI objects for the data.
    std::vector<EditorObjectPtr> editorObjects;
    outErr = data.BuildEditorElements(editorObjects, materials);
    if (!outErr.empty())
    {
        outErr = "Error generating editor GUI elements: " + outErr;
        return;
    }

    //Create and position the editor panel.
    EditorPanel* panel = new EditorPanel(materials, 0.0f, 0.0f);
    panel->AddObjects(editorObjects);
    manager.RootElement = GUIElementPtr(panel);
    manager.RootElement->SetColor(Vector4f(1.0f, 1.0f, 1.0f, 0.5f));
}

void OldOneEditorGUI::Update(float seconds, Vector2i mousePos, bool mouseClicked)
{
    manager.Update(seconds, mousePos, mouseClicked);
}
void OldOneEditorGUI::Render(float frameSeconds, float totalSeconds, Vector2i windowSize)
{
    //Position the panel.
    Vector2f dims = manager.RootElement->GetBounds().GetDimensions();
    manager.RootElement->SetBounds(Box2D(Vector2f(windowSize.x - (dims.x * 0.5f), dims.y * -0.5f),
                                         dims));


    RenderingState(RenderingState::C_NONE, true, true,
                   RenderingState::AT_ALWAYS, 0.0f).EnableState();
    ScreenClearer(false, true, false, Vector4f(0.1f, 0.0f, 0.0f, 0.0f), 1.0f).ClearScreen();

    Camera cam(Vector3f(), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, -1.0f, 0.0f));
    cam.MinOrthoBounds = Vector3f(0.0f, 0.0f, -100.0f);
    cam.MaxOrthoBounds = Vector3f((float)windowSize.x, (float)windowSize.y, 100.0f);
    cam.PerspectiveInfo.Width = windowSize.x;
    cam.PerspectiveInfo.Height = windowSize.y;

    Matrix4f viewM, projM;
    cam.GetViewTransform(viewM);
    cam.GetOrthoProjection(projM);

    manager.Render(frameSeconds, RenderInfo(totalSeconds, &cam, &viewM, &projM));
}