#include "GUIElement.h"

#include "GUIMaterials.h"
#include "../Materials/Data Nodes/DataNodeIncludes.h"


DrawingQuad * GUIElement::quad = 0;


void GUIElement::SetBounds(Box2D newBounds)
{
    Box2D currentBounds = GetBounds();

    ScaleBy(Vector2f(newBounds.GetXSize() / currentBounds.GetXSize(),
                     newBounds.GetYSize() / currentBounds.GetYSize()));
    SetPosition(newBounds.GetCenter());
}


void GUIElement::SetUpQuad(void) const
{
    Box2D bnds = GetBounds();
    bnds.Move(GetPos());
    SetUpQuad(bnds, Depth);
}
void GUIElement::SetUpQuad(const Box2D& bounds, float depth)
{
    GetQuad()->SetPos(bounds.GetCenter());
    GetQuad()->SetDepth(depth);
    GetQuad()->SetSize(bounds.GetDimensions().ComponentProduct(Vector2f(0.5f, -0.5f)));
}

Vector4f GUIElement::GetColor(void) const
{
    auto loc = Params.FloatUniforms.find(GUIMaterials::QuadDraw_Color);
    if (loc == Params.FloatUniforms.end()) return Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    else return *(Vector4f*)&loc->second.Value;
}

void GUIElement::Update(float elapsed, Vector2f relativeMouse)
{
    DidBoundsChange = false;

    if (OnUpdate != 0) OnUpdate(this, relativeMouse, OnUpdate_Data);

    this->CustomUpdate(elapsed, relativeMouse);

    if (!UsesTimeLerp()) return;


    //Update the time lerp value.

    float timeLerp = GetTimeLerp();
    isMousedOver = GetBounds().IsPointInside(relativeMouse);

    //If this element isn't animating, check whether it's being moused over.
    if (CurrentTimeLerpSpeed == 0.0f)
    {
        timeLerp = (isMousedOver ? 0.5f : 0.0f);
    }
    //Otherwise, update its animation.
    else
    {
        timeLerp += elapsed * CurrentTimeLerpSpeed;

        if (timeLerp <= 0.0f ||
            (CurrentTimeLerpSpeed < 0.0f && isMousedOver && timeLerp <= 0.5f))
        {
            CurrentTimeLerpSpeed = 0.0f;
            timeLerp = 0.00001f;
        }
        else if (timeLerp >= 1.0f)
        {
            CurrentTimeLerpSpeed = -CurrentTimeLerpSpeed;
            timeLerp = 0.999999f;
        }
    }

    //Update the time lerp parameter with the new value.
    SetTimeLerp(timeLerp);
}

std::string GUIElement::RenderChild(GUIElement* child, float elapsedTime, const RenderInfo& info) const
{
    Vector2f myPos = GetPos();
    Vector4f myCol = GetColor();

    Vector2f oldPos = child->GetPos();
    float oldDepth = child->Depth;
    Vector4f oldCol = child->GetColor();
    bool oldDidChange = child->DidBoundsChange;

    child->MoveElement(myPos);
    child->Depth += Depth;
    child->SetColor(oldCol.ComponentProduct(myCol));
    
    child->DidBoundsChange = oldDidChange;
    std::string err = child->Render(elapsedTime, info);
    oldDidChange = child->DidBoundsChange;

    child->SetPosition(oldPos);
    child->Depth = oldDepth;
    child->SetColor(oldCol);
    child->DidBoundsChange = oldDidChange;

    return err;
}