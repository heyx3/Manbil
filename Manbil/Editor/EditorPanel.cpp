#include "EditorPanel.h"


void EditorPanel::CustomUpdate(float elapsedTime, Vector2f relMousePos)
{
    //Update the editor objects.
    for (unsigned int i = 0; i < editorObjects.size(); ++i)
    {
        editorObjects[i]->Update(elapsedTime, relMousePos);

        //If the active element changed, replace it in the formatted panel.
        GUIElementPtr activeElement = editorObjects[i]->GetActiveGUIElement();
        if (panel.GetObjects()[i].GUIElementTypeData.Element.get() != activeElement.get())
            panel.ReplaceObject(i, GUIFormatObject(GUIFormatObject::GUIElementType(activeElement)));
    }

    //Now update the GUI objects.
    panel.Update(elapsedTime, relMousePos);
}
std::string EditorPanel::Render(float elapsedTime, const RenderInfo & info)
{
    return panel.Render(elapsedTime, info);
    /*
    std::string err = "";
    for (unsigned int i = 0; i < editorObjects.size(); ++i)
    {
        std::string tempErr = editorObjects[i]->GetActiveGUIElement()->Render(elapsedTime, info);
        if (!tempErr.empty())
            err += "Error rendering object #" + std::to_string(i + 1) +
                    ": " + tempErr + "\n";
    }
    return err;
    */
}

std::string EditorPanel::AddObject(EditorObjectPtr toAdd)
{
    if (toAdd->InitGUIElement(MaterialSet))
    {
        editorObjects.insert(editorObjects.end(), toAdd);
        panel.AddObject(GUIFormatObject(GUIFormatObject::GUIElementType(toAdd->GetActiveGUIElement())));
        return "";
    }
    else
    {
        return EditorObject::ErrorMsg;
    }
}
std::string EditorPanel::AddObjects(const std::vector<EditorObjectPtr> & toAdds)
{
    std::vector<GUIFormatObject> newObjs;
    newObjs.reserve(toAdds.size());
    for (unsigned int i = 0; i < toAdds.size(); ++i)
    {
        if (toAdds[i]->InitGUIElement(MaterialSet))
        {
            newObjs.insert(newObjs.end(),
                           GUIFormatObject(GUIFormatObject::GUIElementType(toAdds[i]->GetActiveGUIElement())));
            editorObjects.insert(editorObjects.end(), toAdds[i]);
        }
        else
        {
            //Make sure to remove all the other elements that were added.
            editorObjects.erase(editorObjects.end() - newObjs.size(), editorObjects.end());
            return std::string("Error adding object #") + std::to_string(i) + ": " + EditorObject::ErrorMsg;
        }
    }

    return "";
}
bool EditorPanel::RemoveObject(EditorObjectPtr toRemove)
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