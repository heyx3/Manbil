#include "GUISlider.h"


Vector2i GUISlider::GetCollisionDimensions(void) const
{
    Vector2f sizeF((float)Bar->GetWidth(), (float)Bar->GetHeight());
    sizeF.MultiplyComponents(BarScale);

    return sizeF.RoundToInt();
}

std::string GUISlider::Render(float elapsedTime, const RenderInfo & info)
{
    //Render the bar. Don't use time lerp value.

    UniformValueF timeLerpV;
    bool usesTimeLerp = UsesTimeLerp();
    if (usesTimeLerp)
    {
        timeLerpV = Params.FloatUniforms[GUIMaterials::DynamicQuadDraw_TimeLerp];
        Params.FloatUniforms.erase(Params.FloatUniforms.find(GUIMaterials::DynamicQuadDraw_TimeLerp));
    }

    Vector2f barPos = ToV2f(center);
    Vector2f barScale = BarScale.ComponentProduct(ToV2f(Vector2i(Bar->GetWidth(), Bar->GetHeight())));

    SetUpQuad(info, barPos, barScale);
    if (!GetQuad()->Render(info, Params, *BarMat))
        return "Error rendering bar: " + BarMat->GetErrorMsg();

    if (usesTimeLerp)
        Params.FloatUniforms[GUIMaterials::DynamicQuadDraw_TimeLerp] = timeLerpV;



    //Render the nub.

    Vector2f nubPos = barPos;
    if (IsVertical) nubPos.y += BasicMath::Lerp(barPos.y - (barScale.y * 0.5f),
                                                barPos.y + (barScale.y * 0.5f),
                                                Value);
    else nubPos.x += BasicMath::Lerp(barPos.x - (barScale.x * 0.5f),
                                     barPos.x + (barScale.x * 0.5f),
                                     Value);
    Vector2f nubScale = NubScale.ComponentProduct(ToV2f(Vector2i(Nub->GetWidth(), Nub->GetHeight())));


    SetUpQuad(info, nubPos, nubScale);
    if (!GetQuad()->Render(info, Params, *NubMat))
        return "Error rendering nub: " + NubMat->GetErrorMsg();

    return "";
}

void GUISlider::OnMouseClick(Vector2i mousePos)
{
    if (IsClickable && IsLocalInsideBounds(mousePos))
    {
        Vector2i dims = GetCollisionDimensions();
        if (IsVertical) Value = BasicMath::Clamp(BasicMath::LerpComponent(-dims.y / 2, dims.y / 2, mousePos.y),
                                                 0.0f, 1.0f);
        else Value = BasicMath::Clamp(BasicMath::LerpComponent(-dims.x / 2, dims.x / 2, mousePos.x),
                                      0.0f, 1.0f);
        
        if (UsesTimeLerp())
        {
            CurrentTimeLerpSpeed = TimeLerpSpeed;
            SetTimeLerp(0.0f);
        }
    }
}
void GUISlider::OnMouseDrag(Vector2i originalPos, Vector2i currentPos)
{
    if (IsClickable && IsLocalInsideBounds(originalPos) || IsLocalInsideBounds(currentPos))
    {
        Vector2i dims = GetCollisionDimensions();
        if (IsVertical) Value = BasicMath::Clamp(BasicMath::LerpComponent(-dims.y / 2, dims.y / 2, currentPos.y),
                                                 0.0f, 1.0f);
        else Value = BasicMath::Clamp(BasicMath::LerpComponent(-dims.x / 2, dims.x / 2, currentPos.x),
                                      0.0f, 1.0f);


        if (UsesTimeLerp() && CurrentTimeLerpSpeed <= 0.0f)
        {
            CurrentTimeLerpSpeed = TimeLerpSpeed;
            SetTimeLerp(0.0f);
        }
    }
}