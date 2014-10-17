#include "GUIFormattedPanel.h"



void GUIFormatObject::MoveObject(MovementData & data)
{
    Vector2f dims = Element->GetCollisionDimensions();
    Vector2f min = data.AutoPosCounter;

    Element->SetBounds(Vector2f(min.x, -(min.y + dims.y)),
                       Vector2f(min.x + dims.x, -min.y));

    if (MoveHorizontal)
        data.AutoPosCounter.x += dims.x;
    if (MoveVertical)
        data.AutoPosCounter.y += dims.y;
    data.AutoPosCounter += SpaceAfter;
}


void GUIFormattedPanel::AddObject(const GUIFormatObject & toAdd)
{
    objects.insert(objects.end(), toAdd);
    RePositionElements();
}
void GUIFormattedPanel::AddRange(const std::vector<GUIFormatObject> & toAdd)
{
    objects.insert(objects.end(), toAdd.begin(), toAdd.end());
    RePositionElements();
}
void GUIFormattedPanel::InsertObject(unsigned int index, const GUIFormatObject & toInsert)
{
    objects.insert(objects.begin() + index, toInsert);
    RePositionElements();
}
void GUIFormattedPanel::InsertRange(unsigned int index, const std::vector<GUIFormatObject> & toInsert)
{
    objects.insert(objects.begin() + index, toInsert.begin(), toInsert.end());
    RePositionElements();
}
void GUIFormattedPanel::RemoveObject(unsigned int index)
{
    objects.erase(objects.begin() + index);
    RePositionElements();
}
void GUIFormattedPanel::ReplaceObject(unsigned int index, const GUIFormatObject & toAdd)
{
    objects.erase(objects.begin() + index);
    objects.insert(objects.begin() + index, toAdd);
    RePositionElements();
}
bool GUIFormattedPanel::ContainsElement(GUIElement* toFind)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].Element.get() == toFind)
        {
            return true;
        }
    }

    return false;
}


void GUIFormattedPanel::ScaleBy(Vector2f scaleAmount)
{
    //Scale the 'extents' vector.
    extents.MultiplyComponents(scaleAmount);

    //Scale each element and move its position to keep it at the same position in the panel.
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        Vector2f elPos = objects[i].Element->GetCollisionCenter();
        objects[i].Element->SetPosition(elPos.ComponentProduct(scaleAmount));
        objects[i].Element->ScaleBy(scaleAmount);
    }
}
void GUIFormattedPanel::SetScale(Vector2f newScale)
{
    Vector2f dims = GetCollisionDimensions();
    Vector2f delta(newScale.x / dims.x, newScale.y / dims.y);
    ScaleBy(delta);
}

void GUIFormattedPanel::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
        objects[i].Element->Update(elapsed, relativeMousePos - objects[i].Element->GetCollisionCenter());
}
std::string GUIFormattedPanel::Render(float elapsedTime, const RenderInfo & info)
{
    Vector2f nPos = -pos;
    std::string err = "";


    //First, render the background.
    if (BackgroundTex.IsValid())
    {
        BackgroundTex.SetPosition(pos);
        BackgroundTex.SetScale(GetCollisionDimensions());
        BackgroundTex.Depth = Depth;
        err = BackgroundTex.Render(elapsedTime, info);

        if (!err.empty()) return "Error rendering background texture: " + err;
    }

    //Instead of returning an error as soon as it is found,
    //    render all sub-elements and collect any errors into one big error string.
    unsigned int line = 0;
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        GUIElementPtr el = objects[i].Element;
        el->MoveElement(pos);
        el->Depth += Depth;
        std::string tempErr = el->Render(elapsedTime, info);
        el->MoveElement(nPos);
        el->Depth -= Depth;

        if (!tempErr.empty())
        {
            if (line > 0) err += "\n";
            line += 1;

            err += std::to_string(line) + ") " + tempErr;
        }
    }


    return err;
}

void GUIFormattedPanel::OnMouseClick(Vector2f mouseP)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        objects[i].Element->OnMouseClick(mouseP - objects[i].Element->GetCollisionCenter());
    }
}
void GUIFormattedPanel::OnMouseDrag(Vector2f oldP, Vector2f currentP)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        Vector2f center = objects[i].Element->GetCollisionCenter();
        objects[i].Element->OnMouseDrag(oldP - center, currentP - center);
    }
}
void GUIFormattedPanel::OnMouseRelease(Vector2f mouseP)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        objects[i].Element->OnMouseRelease(mouseP - objects[i].Element->GetCollisionCenter());
    }
}

void GUIFormattedPanel::RePositionElements()
{
    //We don't know yet how wide/tall the elements will be, so just start at { 0, 0 }.
    MovementData moveDat;
    Vector2f max(-999999.0f, -999999.0f);

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        objects[i].MoveObject(moveDat);
        Vector2f dims = objects[i].Element->GetCollisionDimensions(),
                 center = objects[i].Element->GetCollisionCenter();

        if (objects[i].MoveHorizontal)
            max.x = BasicMath::Max(max.x, moveDat.AutoPosCounter.x);
        else
            max.x = BasicMath::Max(max.x, moveDat.AutoPosCounter.x - objects[i].SpaceAfter.x + dims.x);

        if (objects[i].MoveVertical)
            max.y = BasicMath::Max(max.y, moveDat.AutoPosCounter.y);
        else
            max.y = BasicMath::Max(max.y, moveDat.AutoPosCounter.y - objects[i].SpaceAfter.y + dims.y);
    }

    //Calculate the extents and re-center the elements around the origin.
    extents = max + Vector2f(HorizontalBorder, -VerticalBorder);
    Vector2f delta = extents.ComponentProduct(Vector2f(-0.5f, 0.5f));
    for (unsigned int i = 0; i < objects.size(); ++i)
        objects[i].Element->MoveElement(delta);
}