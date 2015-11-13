#include "GUIElement.h"

#include "GUIMaterials.h"
#include "../Data Nodes/DataNodes.hpp"



bool GUIElement::UsesTimeLerp(void) const
{
    return Params.find(GUIMaterials::DynamicQuadDraw_TimeLerp) != Params.end();
}

float GUIElement::GetTimeLerp(void) const
{
    const Uniform& unf = Params.find(GUIMaterials::DynamicQuadDraw_TimeLerp)->second;
    assert(unf.Type == UT_VALUE_F && unf.Float().GetSize() == 1);

    return unf.Float().GetValue()[0];
}
void GUIElement::SetTimeLerp(float newVal)
{
    Uniform& unf = Params.find(GUIMaterials::DynamicQuadDraw_TimeLerp)->second;
    assert(unf.Type == UT_VALUE_F && unf.Float().GetSize() == 1);
    unf.Float().SetValue(newVal);
}

Vector4f GUIElement::GetColor(void) const
{
    auto found = Params.find(GUIMaterials::QuadDraw_Color);
    if (found == Params.end())
    {
        return Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    }

    const Uniform& unf = found->second;
    assert(unf.Type == UT_VALUE_F && unf.Float().GetSize() == 4);

    //Get the array of floats and just re-interpret it as a vector.
    return *(Vector4f*)(unf.Float().GetValue());
}
void GUIElement::SetColor(Vector4f newCol)
{
    auto found = Params.find(GUIMaterials::QuadDraw_Color);
    if (found == Params.end())
    {
        Params[GUIMaterials::QuadDraw_Color] = Uniform(GUIMaterials::QuadDraw_Color, UT_VALUE_F);
        Params[GUIMaterials::QuadDraw_Color].Float().SetValue(newCol);
    }
    else
    {
        Uniform& unf = found->second;
        assert(unf.Type == UT_VALUE_F && unf.Float().GetSize() == 4);

        unf.Float().SetValue(newCol);
    }
}

void GUIElement::ScaleBy(Vector2f scaleAmount)
{
    DidBoundsChange = true;
    scale.MultiplyComponents(scaleAmount);
}

void GUIElement::SetBounds(Box2D newBounds)
{
    Box2D currentBounds = GetBounds();

    MoveElement(newBounds.GetCenter() - (GetPos() + currentBounds.GetCenter()));
    ScaleBy(Vector2f(newBounds.GetXSize() / currentBounds.GetXSize(),
                     newBounds.GetYSize() / currentBounds.GetYSize()));
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

void GUIElement::Update(float elapsed, Vector2f relativeMouse)
{
    DidBoundsChange = false;

    if (OnUpdate != 0)
    {
        OnUpdate(this, elapsed, relativeMouse, OnUpdate_Data);
    }

    this->CustomUpdate(elapsed, relativeMouse);

    if (!UsesTimeLerp())
    {
        return;
    }


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

void GUIElement::RenderChild(GUIElement* child, float elapsedTime, const RenderInfo& info) const
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
    child->Render(elapsedTime, info);
    oldDidChange = child->DidBoundsChange;

    child->SetPosition(oldPos);
    child->Depth = oldDepth;
    child->SetColor(oldCol);
    child->DidBoundsChange = oldDidChange;
}