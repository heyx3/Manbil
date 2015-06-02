#include "GUITexture.h"

#include "../GUIMaterials.h"


Box2D GUITexture::GetBounds(void) const
{
    Vector2f dims = GetScale();
    if (tex != 0)
    {
        if (rotation == 0.0f)
        {
            dims = ToV2f(Vector2u(tex->GetWidth(), tex->GetHeight()));
            dims.MultiplyComponents(GetScale());
        }
        else
        {
            float diameter = 2.0f * ToV2f(Vector2u(tex->GetWidth(), tex->GetHeight())).Length();
            dims = Vector2f(diameter, diameter);
        }
    }

    return Box2D(Vector2f(), dims);
}


void GUITexture::SetRotation(float r)
{
    DidBoundsChange = DidBoundsChange || ((r == 0.0f) != (rotation == 0.0f));
    rotation = r;
}

#pragma warning(disable: 4100)
void GUITexture::Render(float elapsed, const RenderInfo& info)
{
    if (Mat == 0)
    {
        return;
    }

    if (tex == 0)
    {
        Params.Texture2Ds[GUIMaterials::QuadDraw_Texture2D].Texture = INVALID_RENDER_OBJ_HANDLE;
    }
    else
    {
        Params.Texture2Ds[GUIMaterials::QuadDraw_Texture2D].Texture = tex->GetTextureHandle();
    }

    SetUpQuad();
    DrawingQuad::GetInstance()->Rotate(rotation);
    GetQuad()->Render(info, Params, *Mat);
    DrawingQuad::GetInstance()->SetRotation(0.0f);
}
#pragma warning(default: 4100)

void GUITexture::OnMouseClick(Vector2f mousePos)
{
    if (IsButton && GetBounds().IsPointInside(mousePos))
    {
        isBeingClicked = true;
        CurrentTimeLerpSpeed = TimeLerpSpeed;
        if (OnClicked != 0)
        {
            OnClicked(this, mousePos, OnClicked_pData);
        }
    }
}
void GUITexture::OnMouseRelease(Vector2f mousePos)
{
    if (IsButton && isBeingClicked)
    {
        isBeingClicked = false;
        if (OnReleased != 0)
        {
            OnReleased(this, mousePos, OnReleased_pData);
        }
    }
}