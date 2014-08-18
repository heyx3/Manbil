#include "GUITexture.h"

#include "../GUIMaterials.h"


Vector2i GUITexture::GetCollisionDimensions(void) const
{
    if (Tex == 0) return Vector2i();

    Vector2f scaled = Scale.ComponentProduct(Vector2f((float)Tex->GetWidth(), (float)Tex->GetHeight()));
    return scaled.RoundToInt();
}

std::string GUITexture::Render(float elapsed, const RenderInfo & info)
{
    if (Mat == 0) return "No material!";

    SetUpQuad(info, Vector2f((float)center.x, (float)center.y),
              Scale.ComponentProduct(Vector2f((float)Tex->GetWidth(), (float)Tex->GetHeight())));

    Params.Texture2DUniforms[GUIMaterials::QuadDraw_Texture2D].Texture = Tex->GetTextureHandle();

    return (GetQuad()->Render(info, Params, *Mat) ?
                "" :
                "Error rendering GUITexture: " + Mat->GetErrorMsg());
}

void GUITexture::OnMouseClick(Vector2i mousePos)
{
    if (IsButton && IsLocalInsideBounds(mousePos))
    {
        isBeingClicked = true;
        CurrentTimeLerpSpeed = TimeLerpSpeed;
        if (OnClicked != 0) OnClicked(this, mousePos, OnClicked_pData);
    }
}
void GUITexture::OnMouseRelease(Vector2i mousePos)
{
    if (IsButton && isBeingClicked)
    {
        isBeingClicked = false;
        CurrentTimeLerpSpeed = -TimeLerpSpeed;
        if (OnReleased != 0) OnReleased(this, mousePos, OnReleased_pData);
    }
}