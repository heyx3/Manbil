#include "EditorPanel.h"


void EditorPanel::CustomUpdate(float elapsedTime, Vector2f relMousePos)
{
    for (unsigned int i = 0; i < editorObjects.size(); ++i)
    {
        editorObjects[i]->Update(elapsedTime, relMousePos);
        GUIElementPtr activeElement = editorObjects[i]->GetActiveGUIElement();
        if (activeElement.get() != 0)
            activeElement->Update(elapsedTime, relMousePos - activeElement->GetCollisionCenter());
    }
}
std::string EditorPanel::Render(float elapsedTime, const RenderInfo & info)
{
    std::string err = "";
    for (unsigned int i = 0; i < editorObjects.size(); ++i)
    {
        std::string tempErr = editorObjects[i]->GetActiveGUIElement()->Render(elapsedTime, info);
        if (!tempErr.empty())
            err += "Error rendering object #" + std::to_string(i + 1) +
                    ": " + tempErr + "\n";
    }
    return err;
}

std::string EditorPanel::AddObject(std::shared_ptr<EditorObject> toAdd)
{
    editorObjects.insert(editorObjects.end(), toAdd);
    if (toAdd->InitGUIElement(MaterialSet))
    {
        panel.AddObject(GUIFormatObject(GUIFormatObject::GUIElementType(toAdd->GetActiveGUIElement())));
        return "";
    }
    else
    {
        return EditorObject::ErrorMsg;
    }
}
bool EditorPanel::RemoveObject(std::shared_ptr<EditorObject> toRemove)
{
    bool found = false;

    for (unsigned int i = 0; i < editorObjects.size(); ++i)
    {
        if (editorObjects[i].get() == toRemove.get())
        {
            editorObjects.erase(editorObjects.begin() + i);
            found = true;
            panel.RemoveObject(i);
            --i;
        }
    }

    return found;
}