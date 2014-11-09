#include "GUITexture.h"

#include "../GUIMaterials.h"


Box2D GUITexture::GetBounds(void) const
{
    Vector2f dims;
    if (tex != 0)
    {
        dims = ToV2f(Vector2u(tex->GetWidth(), tex->GetHeight()));
        dims.MultiplyComponents(GetScale());
    }

    return Box2D(Vector2f(), dims);
}

std::string GUITexture::Render(float elapsed, const RenderInfo & info)
{
    if (tex == 0 || Mat == 0) return "Texture or material is not set!";

    SetUpQuad();
    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture = tex->GetTextureHandle();
    return (GetQuad()->Render(info, Params, *Mat) ?
                "" :
                "Error rendering GUITexture: " + Mat->GetErrorMsg());
}

void GUITexture::OnMouseClick(Vector2f mousePos)
{
    if (IsButton && GetBounds().IsPointInside(mousePos))
    {
        isBeingClicked = true;
        CurrentTimeLerpSpeed = TimeLerpSpeed;
        if (OnClicked != 0) OnClicked(this, mousePos, OnClicked_pData);
    }
}
void GUITexture::OnMouseRelease(Vector2f mousePos)
{
    if (IsButton && isBeingClicked)
    {
        isBeingClicked = false;
        if (OnReleased != 0) OnReleased(this, mousePos, OnReleased_pData);
    }
}