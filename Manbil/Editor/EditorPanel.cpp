#include "EditorPanel.h"


void EditorPanel::CustomUpdate(float elapsedTime, Vector2f relMousePos)
{
    //Update the editor objects.
    for (unsigned int i = 0; i < editorObjects.size(); ++i)
    {
        editorObjects[i]->Update(elapsedTime, relMousePos);

        //If the active element changed, replace it in the formatted panel.
        GUIElementPtr activeElement = editorObjects[i]->GetActiveGUIElement();
        if (panel.GetObjects()[i].Element.get() != activeElement.get())
            panel.ReplaceObject(i, GUIFormatObject(activeElement, editorObjects[i]->GetMoveHorizontally(),
                                                   editorObjects[i]->GetMoveVertically(),
                                                   editorObjects[i]->Offset));
    }

    //Now update the GUI objects.
    panel.Update(elapsedTime, relMousePos);
}
std::string EditorPanel::Render(float elapsedTime, const RenderInfo & info)
{
    return panel.Render(elapsedTime, info);
}

std::string EditorPanel::AddObject(EditorObjectPtr toAdd)
{
    if (toAdd->InitGUIElement(MaterialSet))
    {
        editorObjects.insert(editorObjects.end(), toAdd);
        panel.AddObject(GUIFormatObject(toAdd->GetActiveGUIElement(), toAdd->GetMoveHorizontally(),
                                        toAdd->GetMoveVertically(), toAdd->Offset));
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
                           GUIFormatObject(toAdds[i]->GetActiveGUIElement(),
                                           toAdds[i]->GetMoveHorizontally(),
                                           toAdds[i]->GetMoveVertically(),
                                           toAdds[i]->Offset));
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