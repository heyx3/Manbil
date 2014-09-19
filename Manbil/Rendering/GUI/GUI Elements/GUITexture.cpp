#include "GUITexture.h"

#include "../GUIMaterials.h"


Vector2f GUITexture::GetCollisionDimensions(void) const
{
    if (Tex == 0) return Vector2f();

    return Scale.ComponentProduct(Vector2f((float)Tex->GetWidth(), (float)Tex->GetHeight()));
}

std::string GUITexture::Render(float elapsed, const RenderInfo & info)
{
    if (Tex == 0 || Mat == 0) return "Texture or material is not set!";

    SetUpQuad(Vector2f((float)center.x, (float)center.y), Depth,
              Scale.ComponentProduct(ToV2f(Vector2u(Tex->GetWidth(), Tex->GetHeight()))));

    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture = Tex->GetTextureHandle();

    return (GetQuad()->Render(info, Params, *Mat) ?
                "" :
                "Error rendering GUITexture: " + Mat->GetErrorMsg());
}

void GUITexture::OnMouseClick(Vector2f mousePos)
{
    if (IsButton && IsLocalInsideBounds(mousePos))
    {
        isBeingClicked = true;
        CurrentTimeLerpSpeed = TimeLerpSpeed;
        SetTimeLerp(0.0f);
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