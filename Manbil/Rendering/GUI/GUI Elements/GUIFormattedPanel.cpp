#include "GUIFormattedPanel.h"



void GUIFormatObject::MoveObject(MovementData & data)
{
    GUIElement* el;
    float spaceAfter, xOffset;
    Vector2f dims, min;

    switch (Type)
    {
        case OT_GUIELEMENT:

            el = GUIElementTypeData.Element;
            spaceAfter = GUIElementTypeData.SpaceAfter;
            xOffset = GUIElementTypeData.XOffset;

            dims = el->GetCollisionDimensions();
            min = Vector2f(data.AutoPosCounter.x + xOffset, data.AutoPosCounter.y);
            el->SetBounds(min, Vector2f(min.x + dims.x, min.y + dims.y));

            data.AutoPosCounter.y += dims.y + spaceAfter;
            data.Width = BasicMath::Max(data.Width, xOffset + dims.x);

            break;

        case OT_VERTBREAK:
            data.AutoPosCounter = Vector2f(data.AutoPosCounter.x,
                                           data.AutoPosCounter.y + VertBreakTypeData.SpaceAfter);
            break;
        case OT_HORZBREAK:
            data.AutoPosCounter.x += data.Width + HorzBreakTypeData.XOffset;
            data.AutoPosCounter.y = HorzBreakTypeData.VerticalBorder;
            data.Width = 0.0f;
            break;


        default:
            assert(false);
            break;
    }
}


void GUIFormattedPanel::ScaleBy(Vector2f scaleAmount)
{
    //Scale the 'extents' vector.
    extents = Vector2f(extents.x, extents.y).ComponentProduct(scaleAmount);

    //Scale each element's position to move it relative to this panel's center.
    for (unsigned int i = 0; i < Objects.size(); ++i)
    {
        if (Objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = Objects[i].GUIElementTypeData.Element;
            Vector2f elPos = el->GetCollisionCenter();
            el->SetPosition(Vector2f(elPos.x, elPos.y).ComponentProduct(scaleAmount));
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

    //Auto-arrange each element before it's updated.
    MovementData moveDat;

    for (unsigned int i = 0; i < Objects.size(); ++i)
    {
        if (Objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            Objects[i].MoveObject(moveDat);

            GUIElement* el = Objects[i].GUIElementTypeData.Element;
            Vector2f relPos = relativeMousePos - el->GetCollisionCenter();
            el->MoveElement(pos);
            el->Update(elapsed, relPos);
            el->MoveElement(nPos);
        }
    }
}
std::string GUIFormattedPanel::Render(float elapsedTime, const RenderInfo & info)
{
    Vector2f nPos = -pos;

    //Instead of returning an error as soon as it is found,
    //    render all sub-elements and collect any errors into one big error string.
    std::string err = "";
    unsigned int line = 0;

    for (unsigned int i = 0; i < Objects.size(); ++i)
    {
        if (Objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = Objects[i].GUIElementTypeData.Element;
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
    for (unsigned int i = 0; i < Objects.size(); ++i)
    {
        if (Objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = Objects[i].GUIElementTypeData.Element;
            el->OnMouseClick(mouseP - el->GetCollisionCenter());
        }
    }
}
void GUIFormattedPanel::OnMouseDrag(Vector2f oldP, Vector2f currentP)
{
    for (unsigned int i = 0; i < Objects.size(); ++i)
    {
        if (Objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = Objects[i].GUIElementTypeData.Element;
            Vector2f center = el->GetCollisionCenter();
            el->OnMouseDrag(oldP - center, currentP - center);
        }
    }
}
void GUIFormattedPanel::OnMouseRelease(Vector2f mouseP)
{
    for (unsigned int i = 0; i < Objects.size(); ++i)
    {
        if (Objects[i].Type == GUIFormatObject::OT_GUIELEMENT)
        {
            GUIElement* el = Objects[i].GUIElementTypeData.Element;
            el->OnMouseRelease(mouseP - el->GetCollisionCenter());
        }
    }
}