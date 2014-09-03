#include "GUIFormattedPanel.h"



void GUIFormatObject::MoveObject(MovementData & data)
{
    GUIElement* el;
    float xOffset;
    Vector2f dims, min;

    switch (Type)
    {
        case OT_GUIELEMENT:

            el = GUIElementTypeData.Element;
            xOffset = GUIElementTypeData.XOffset;

            dims = el->GetCollisionDimensions();
            min = Vector2f(data.AutoPosCounter.x + xOffset, data.AutoPosCounter.y);
            el->SetBounds(min, min + dims);

            data.AutoPosCounter.y += dims.y + SpaceAfter;
            data.Width = BasicMath::Max(data.Width, xOffset + dims.x);

            break;

        case OT_VERTBREAK:
            data.AutoPosCounter = Vector2f(data.AutoPosCounter.x,
                                           data.AutoPosCounter.y + SpaceAfter);
            break;
        case OT_HORZBREAK:
            data.AutoPosCounter.x += data.Width + HorzBreakTypeData.XOffset;
            data.AutoPosCounter.y = SpaceAfter;
            data.Width = 0.0f;
            break;


        default:
            assert(false);
            break;
    }
}


void GUIFormattedPanel::AddObject(const GUIFormatObject & toAdd)
{
    objects.insert(objects.end(), toAdd);
    RePositionElements();
}
void GUIFormattedPanel::RemoveObject(unsigned int index)
{
    objects.erase(objects.begin() + index);
    RePositionElements();
}
bool GUIFormattedPanel::ContainsElement(GUIElement* toFind)
{
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].Type == GUIFormatObject::OT_GUIELEMENT &&
            objects[i].GUIElementTypeData.Element == toFind)
        {
            return true;
        }
    }

    return false;
}


void GUIFormattedPanel::ScaleBy(Vector2f scaleAmount)
{
    //Scale the 'extents' vector.
    extents = Vector2f(extents.x, extents.y).ComponentProduct(scaleAmount);

    //Scale each element and move its position to keep it at the same position in the panel.
    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = objects[i].GUIElementTypeData.Element;
            Vector2f elPos = el->GetCollisionCenter();
            el->SetPosition(Vector2f(elPos.x, elPos.y).ComponentProduct(scaleAmount));
            el->ScaleBy(scaleAmount);
        }
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
    Vector2f nPos = -pos;

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = objects[i].GUIElementTypeData.Element;

            el->MoveElement(pos);
            el->Update(elapsed, relativeMousePos - el->GetCollisionCenter());
            el->MoveElement(nPos);
        }
    }
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
        if (objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = objects[i].GUIElementTypeData.Element;
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
    }


    return err;
}

void GUIFormattedPanel::OnMouseClick(Vector2f mouseP)
{
    Vector2f nPos = -pos;

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = objects[i].GUIElementTypeData.Element;
            el->MoveElement(pos);
            el->OnMouseClick(mouseP - el->GetCollisionCenter());
            el->MoveElement(nPos);
        }
    }
}
void GUIFormattedPanel::OnMouseDrag(Vector2f oldP, Vector2f currentP)
{
    Vector2f nPos = -pos;

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = objects[i].GUIElementTypeData.Element;
            el->MoveElement(pos);
            Vector2f center = el->GetCollisionCenter();
            el->OnMouseDrag(oldP - center, currentP - center);
            el->MoveElement(nPos);
        }
    }
}
void GUIFormattedPanel::OnMouseRelease(Vector2f mouseP)
{
    Vector2f nPos = -pos;

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = objects[i].GUIElementTypeData.Element;
            el->MoveElement(pos);
            el->OnMouseRelease(mouseP - el->GetCollisionCenter());
            el->MoveElement(nPos);
        }
    }
}

void GUIFormattedPanel::RePositionElements()
{
    //We don't know yet how wide/tall the elements will be, so just start at { 0, 0 }.
    MovementData moveDat;
    Vector2f maxPos(0.0f, 0.0f);

    for (unsigned int i = 0; i < objects.size(); ++i)
    {
        objects[i].MoveObject(moveDat);

        maxPos.x = BasicMath::Max(maxPos.x, moveDat.AutoPosCounter.x + moveDat.Width);
        maxPos.y = BasicMath::Max(maxPos.y, moveDat.AutoPosCounter.y);
    }

    //Calculate the extents and re-center the elements around the origin.
    extents = maxPos + Vector2f(HorizontalBorder, VerticalBorder);
    Vector2f delta = extents * -0.5f;//TODO: Might have to modify this delta based on the horizontal/vertical border.
    for (unsigned int i = 0; i < objects.size(); ++i)
        if (objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
            objects[i].GUIElementTypeData.Element->MoveElement(delta);
}