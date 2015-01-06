#include "GUIFormattedPanel.h"



void GUIFormatObject::MoveObject(MovementData & data)
{
    Box2D bnds = Element->GetBounds();
    Vector2f min = data.AutoPosCounter;

    Element->SetBounds(Box2D(min.x, min.x + bnds.GetXSize(),
                             min.y, (min.y + bnds.GetYSize())));

    if (MoveHorizontal)
        data.AutoPosCounter.x += bnds.GetXSize();
    if (MoveVertical)
        data.AutoPosCounter.y += bnds.GetYSize();
    data.AutoPosCounter += SpaceAfter;
}


Box2D GUIFormattedPanel::GetBounds(void) const
{
    return Box2D(Vector2f(), dimensions);
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

bool GUIFormattedPanel::GetDidBoundsChangeDeep(void) const
{
    if (DidBoundsChange) return true;

    for (unsigned int i = 0; i < objects.size(); ++i)
        if (objects[i].Element->GetDidBoundsChangeDeep())
            return true;

    return false;
}
void GUIFormattedPanel::ClearDidBoundsChangeDeep(void)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
        objects[i].Element->ClearDidBoundsChangeDeep();
}

void GUIFormattedPanel::ScaleBy(Vector2f scaleAmount)
{
    GUIElement::ScaleBy(scaleAmount);
    dimensions.MultiplyComponents(scaleAmount);

    //Scale each element and move its position to keep it at the same position in the panel.
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        Vector2f elPos = objects[i].Element->GetPos();
        objects[i].Element->SetPosition(elPos.ComponentProduct(scaleAmount));
        objects[i].Element->ScaleBy(scaleAmount);
    }
}
void GUIFormattedPanel::SetScale(Vector2f newScale)
{
    Vector2f oldScale = GetScale();
    Vector2f delta(newScale.x / oldScale.x, newScale.y / oldScale.y);
    ScaleBy(delta);
}

void GUIFormattedPanel::CustomUpdate(float elapsed, Vector2f relativeMousePos)
{
    if (BackgroundTex.IsValid())
        BackgroundTex.Update(elapsed, relativeMousePos);

    bool changed = false;
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        changed = changed || objects[i].Element->GetDidBoundsChangeDeep();
        objects[i].Element->Update(elapsed, relativeMousePos - objects[i].Element->GetPos());
        changed = changed || objects[i].Element->GetDidBoundsChangeDeep();
    }

    if (changed) RePositionElements();
}
std::string GUIFormattedPanel::Render(float elapsedTime, const RenderInfo & info)
{
    std::string err = "";


    //First, render the background.
    if (BackgroundTex.IsValid())
    {
        BackgroundTex.SetBounds(GetBounds());
        BackgroundTex.Depth = -0.001f;
        err = RenderChild(&BackgroundTex, elapsedTime, info);
        if (!err.empty()) return "Error rendering background texture: " + err;
    }

    //Instead of returning an error as soon as it is found,
    //    render all sub-elements and collect any errors into one big error string.
    unsigned int line = 0;
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        GUIElementPtr el = objects[i].Element;
        std::string tempErr = RenderChild(el.get(), elapsedTime, info);
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
        objects[i].Element->OnMouseClick(mouseP - objects[i].Element->GetPos());
    }
}
void GUIFormattedPanel::OnMouseDrag(Vector2f oldP, Vector2f currentP)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        Vector2f center = objects[i].Element->GetPos();
        objects[i].Element->OnMouseDrag(oldP - center, currentP - center);
    }
}
void GUIFormattedPanel::OnMouseRelease(Vector2f mouseP)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        objects[i].Element->OnMouseRelease(mouseP - objects[i].Element->GetPos());
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
        Box2D bnds = objects[i].Element->GetBounds();

        if (objects[i].MoveHorizontal)
            max.x = Mathf::Max(max.x, moveDat.AutoPosCounter.x);
        else
            max.x = Mathf::Max(max.x, moveDat.AutoPosCounter.x -
                                            objects[i].SpaceAfter.x +
                                            bnds.GetXSize());

        if (objects[i].MoveVertical)
            max.y = Mathf::Max(max.y, moveDat.AutoPosCounter.y);
        else
            max.y = Mathf::Max(max.y, moveDat.AutoPosCounter.y -
                                            objects[i].SpaceAfter.y +
                                            bnds.GetYSize());

        objects[i].Element->ClearDidBoundsChangeDeep();
    }

    //Calculate the extents and re-center the elements around the origin.
    dimensions = max + Vector2f(HorizontalBorder, VerticalBorder);
    Vector2f delta = dimensions.ComponentProduct(Vector2f(-0.5f, -0.5f)) +
                     (Vector2f(HorizontalBorder, VerticalBorder) * 0.5f);
    for (unsigned int i = 0; i < objects.size(); ++i)
        objects[i].Element->MoveElement(delta);

    DidBoundsChange = true;
}