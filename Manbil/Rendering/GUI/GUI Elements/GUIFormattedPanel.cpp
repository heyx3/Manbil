#include "GUIFormattedPanel.h"



//A counter indicating where the next element in the layout should be placed.
//Used in the GUIFormattedPanel's algorithm for laying out a scene.
struct MovementData
{
    Vector2f Pos = Vector2f();
};


GUIFormatObject::GUIFormatObject(GUIElementPtr element,
                                 bool moveHorz, bool moveVert,
                                 Vector2f spaceAfter)
    : Element(element), SpaceAfter(spaceAfter),
      MoveHorizontal(moveHorz), MoveVertical(moveVert)
{

}

void GUIFormatObject::MoveObject(MovementData& data)
{
    Vector2f dims = Element->GetBounds().GetDimensions();
    Vector2f min = data.Pos;

    Element->SetBounds(Box2D(min.x, min.x + dims.x,
                             min.y, min.y + dims.y));

    if (MoveHorizontal)
    {
        data.Pos.x += dims.x;
    }
    if (MoveVertical)
    {
        data.Pos.y += dims.y;
    }
    data.Pos += SpaceAfter;
}


GUIFormattedPanel::GUIFormattedPanel(float horizontalBorder, float verticalBorder,
                                     GUITexture background, float timeLerpSpeed)
    : HorizontalBorder(horizontalBorder), VerticalBorder(verticalBorder),
      BackgroundTex(background), GUIElement(UniformDictionary(), timeLerpSpeed)
{

}

Box2D GUIFormattedPanel::GetBounds(void) const
{
    return Box2D(Vector2f(), dimensions);
}

void GUIFormattedPanel::AddObject(const GUIFormatObject& toAdd)
{
    objects.push_back(toAdd);
    RePositionElements();
}
void GUIFormattedPanel::AddRange(const std::vector<GUIFormatObject>& toAdd)
{
    objects.insert(objects.end(), toAdd.begin(), toAdd.end());
    RePositionElements();
}
void GUIFormattedPanel::InsertObject(unsigned int index, const GUIFormatObject& toInsert)
{
    objects.insert(objects.begin() + index, toInsert);
    RePositionElements();
}
void GUIFormattedPanel::InsertRange(unsigned int index, const std::vector<GUIFormatObject>& toInsert)
{
    objects.insert(objects.begin() + index, toInsert.begin(), toInsert.end());
    RePositionElements();
}
void GUIFormattedPanel::RemoveObject(unsigned int index)
{
    objects.erase(objects.begin() + index);
    RePositionElements();
}
void GUIFormattedPanel::ReplaceObject(unsigned int index, const GUIFormatObject& toAdd)
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

const GUIFormatObject& GUIFormattedPanel::GetFormatObject(unsigned int index) const
{
    assert(index < objects.size());
    return objects[index];
}
GUIFormatObject& GUIFormattedPanel::GetFormatObject(unsigned int index)
{
    assert(index < objects.size());
    return objects[index];
}

bool GUIFormattedPanel::GetDidBoundsChangeDeep(void) const
{
    if (DidBoundsChange)
    {
        return true;
    }

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].Element->GetDidBoundsChangeDeep())
        {
            return true;
        }
    }

    return false;
}
void GUIFormattedPanel::ClearDidBoundsChangeDeep(void)
{
    DidBoundsChange = false;
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        objects[i].Element->ClearDidBoundsChangeDeep();
    }
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
    {
        BackgroundTex.Update(elapsed, relativeMousePos);
    }

    bool changed = DidBoundsChange;
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        changed = changed || objects[i].Element->GetDidBoundsChangeDeep();
        objects[i].Element->Update(elapsed, relativeMousePos - objects[i].Element->GetPos());
        changed = changed || objects[i].Element->GetDidBoundsChangeDeep();
    }

    if (changed)
    {
        RePositionElements();
    }
}
void GUIFormattedPanel::Render(float elapsedTime, const RenderInfo& info)
{
    //First, render the background.
    if (BackgroundTex.IsValid())
    {
        BackgroundTex.SetBounds(GetBounds());
        BackgroundTex.Depth = -0.001f;
        RenderChild(&BackgroundTex, elapsedTime, info);
    }

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        GUIElementPtr el = objects[i].Element;
        RenderChild(objects[i].Element.get(), elapsedTime, info);
    }
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
        Vector2f dims = objects[i].Element->GetBounds().GetDimensions();

        if (objects[i].MoveHorizontal)
        {
            max.x = Mathf::Max(max.x, moveDat.Pos.x);
        }
        else
        {
            max.x = Mathf::Max(max.x, moveDat.Pos.x - objects[i].SpaceAfter.x + dims.x);
        }

        if (objects[i].MoveVertical)
        {
            max.y = Mathf::Max(max.y, moveDat.Pos.y);
        }
        else
        {
            max.y = Mathf::Max(max.y, moveDat.Pos.y - objects[i].SpaceAfter.y + dims.y);
        }
    }

    //Calculate the extents and re-center the elements around the origin.
    dimensions = max + Vector2f(HorizontalBorder, VerticalBorder);
    Vector2f delta = dimensions.ComponentProduct(Vector2f(-0.5f, -0.5f)) +
                                                 (Vector2f(HorizontalBorder, VerticalBorder) * 0.5f);
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        objects[i].Element->MoveElement(delta);
        objects[i].Element->ClearDidBoundsChangeDeep();
    }

    DidBoundsChange = true;
}