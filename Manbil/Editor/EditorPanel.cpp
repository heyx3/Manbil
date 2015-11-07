#include "EditorPanel.h"



EditorPanel::EditorPanel(EditorMaterialSet& set, float horizontalBorder, float verticalBorder)
    : MaterialSet(set),
        panel(horizontalBorder, verticalBorder,
            GUITexture(set.GetStaticMatParams(&set.PanelBackgroundTex),
                        &set.PanelBackgroundTex,
                        set.GetStaticMaterial(&set.PanelBackgroundTex))),
        GUIElement(UniformDictionary()) { }


std::string EditorPanel::AddObject(EditorObjectPtr toAdd, unsigned int index)
{
    std::string err = toAdd->InitGUIElement(MaterialSet);
    if (err.empty())
    {
        editorObjects.insert(editorObjects.begin() + index, toAdd);
        panel.InsertObject(index,
                           GUIFormatObject(toAdd->GetActiveGUIElement(),
                                           toAdd->GetMoveHorizontally(), toAdd->GetMoveVertically(),
                                           toAdd->Offset));
        return "";
    }
    else
    {
        return err;
    }
}

std::string EditorPanel::AddObjects(const std::vector<EditorObjectPtr>& toAdd)
{
    return AddObjects(toAdd, editorObjects.size());
}
std::string EditorPanel::AddObjects(const std::vector<EditorObjectPtr>& toAdds, unsigned int startIndex)
{
    std::vector<GUIFormatObject> newObjs;
    newObjs.reserve(toAdds.size());

    for (unsigned int i = 0; i < toAdds.size(); ++i)
    {
        std::string err = toAdds[i]->InitGUIElement(MaterialSet);
        if (err.empty())
        {
            newObjs.insert(newObjs.begin() + startIndex + i,
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
            return std::string("Error adding object #") + std::to_string(i) + ": " + err;
        }
    }

    panel.InsertRange(startIndex, newObjs);

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

bool EditorPanel::GetDidBoundsChangeDeep(void) const
{
    return DidBoundsChange || panel.GetDidBoundsChangeDeep();
}
void EditorPanel::ClearDidBoundsChangeDeep(void)
{
    DidBoundsChange = false;
    panel.ClearDidBoundsChangeDeep();
}


Vector2f EditorPanel::GetPos(void) const
{
    return panel.GetPos();
}
Box2D EditorPanel::GetBounds(void) const
{
    Box2D bounds = panel.GetBounds();
    bounds.Move(panel.GetPos());
    return bounds;
}
void EditorPanel::MoveElement(Vector2f moveAmount)
{
    GUIElement::MoveElement(moveAmount);
    panel.MoveElement(moveAmount);
}
void EditorPanel::SetPosition(Vector2f newPos)
{
    GUIElement::SetPosition(newPos);
    panel.SetPosition(newPos);
}
void EditorPanel::ScaleBy(Vector2f scaleAmount)
{
    GUIElement::ScaleBy(scaleAmount);
    panel.ScaleBy(scaleAmount);
}
void EditorPanel::SetScale(Vector2f newScale)
{
    GUIElement::SetScale(newScale);
    panel.SetScale(newScale);
}
void EditorPanel::OnMouseClick(Vector2f relativeMousePos)
{
    panel.OnMouseClick(relativeMousePos);
}
void EditorPanel::OnMouseDrag(Vector2f originalMPos, Vector2f currentMPos)
{
    panel.OnMouseDrag(originalMPos, currentMPos);
}
void EditorPanel::OnMouseRelease(Vector2f relativeMousePos)
{
    panel.OnMouseRelease(relativeMousePos);
}

void EditorPanel::CustomUpdate(float elapsedTime, Vector2f relMousePos)
{
    //Update the editor objects.
    for (unsigned int i = 0; i < editorObjects.size(); ++i)
    {
        //If the active element changed, replace it in the formatted panel.
        if (editorObjects[i]->Update(elapsedTime, relMousePos))
        {
            panel.ReplaceObject(i, GUIFormatObject(editorObjects[i]->GetActiveGUIElement(),
                                                   editorObjects[i]->GetMoveHorizontally(),
                                                   editorObjects[i]->GetMoveVertically(),
                                                   editorObjects[i]->Offset));
            DidBoundsChange = true;
        }
    }

    //Now update the GUI objects.
    panel.Update(elapsedTime, relMousePos);
}
void EditorPanel::Render(float elapsedTime, const RenderInfo& info)
{
    Vector4f col = panel.GetColor();
    panel.SetColor(col.ComponentProduct(GetColor()));
    panel.Render(elapsedTime, info);
    panel.SetColor(col);
}