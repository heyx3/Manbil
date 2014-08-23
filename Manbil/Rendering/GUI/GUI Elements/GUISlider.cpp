#include "GUISlider.h"


Vector2f GUISlider::GetCollisionDimensions(void) const
{
    return Bar.GetScale().ComponentProduct(Vector2f((float)Bar.Tex->GetWidth(), (float)Bar.Tex->GetHeight()));
}


void GUISlider::SetScale(Vector2f newScale)
{
    Vector2f oldBarScale = Bar.GetScale();
    Vector2f deltaScale(newScale.x / oldBarScale.x, newScale.y / oldBarScale.y);

    Bar.SetScale(newScale);
    Nub.ScaleBy(deltaScale);
}

float GUISlider::GetNewValue(Vector2f mousePos) const
{
    Vector2f barBounds = Bar.GetCollisionDimensions();

    if (IsVertical)
    {
        return BasicMath::Clamp(BasicMath::LerpComponent(-barBounds.y * 0.5f,
                                                         barBounds.y * 0.5f,
                                                         mousePos.y),
                                0.0f, 1.0f);
    }
    else
    {
        return BasicMath::Clamp(BasicMath::LerpComponent(-barBounds.x * 0.5f,
                                                         barBounds.x * 0.5f,
                                                         mousePos.x),
                                0.0f, 1.0f);
    }
}

std::string GUISlider::Render(float elapsedTime, const RenderInfo & info)
{
    Vector4f myCol = *(Vector4f*)&Params.FloatUniforms[GUIMaterials::QuadDraw_Color].Value;


    //Render the bar.

    Bar.SetPosition(center);
    Bar.Depth = Depth;

    Vector4f oldCol = *(Vector4f*)&Bar.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].Value;
    Bar.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(oldCol.ComponentProduct(myCol));

    std::string err = Bar.Render(elapsedTime, info);
    Bar.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(oldCol);

    if (!err.empty()) return "Error rendering slider bar: " + err;


    //Render the nub.

    Vector2f dims = GetCollisionDimensions();
    Nub.SetPosition(center +
                     (IsVertical ?
                        Vector2f(0.0f, BasicMath::Lerp(-dims.y * 0.5f, dims.y * 0.5f, Value)) :
                        Vector2f(BasicMath::Lerp(-dims.x * 0.5f, dims.x * 0.5f, Value))));
    Nub.Depth = Depth + 0.00001f;

    oldCol = *(Vector4f*)&Nub.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].Value;
    Nub.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(oldCol.ComponentProduct(myCol));

    err = Nub.Render(elapsedTime, info);
    Nub.Params.FloatUniforms[GUIMaterials::QuadDraw_Color].SetValue(oldCol);

    if (!err.empty()) return "Error rendering slider nub: " + err;


    return "";
}

void GUISlider::OnMouseClick(Vector2f mousePos)
{
    if (IsClickable && IsLocalInsideBounds(mousePos))
    {
        Vector2f dims = GetCollisionDimensions();
        Value = GetNewValue(mousePos);
        
        if (UsesTimeLerp())
        {
            CurrentTimeLerpSpeed = TimeLerpSpeed;
            SetTimeLerp(0.0f);
        }

        Nub.OnMouseClick(mousePos - Nub.GetCollisionCenter());
        Bar.OnMouseClick(mousePos - Bar.GetCollisionCenter());
    }
}
void GUISlider::OnMouseDrag(Vector2f originalPos, Vector2f currentPos)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
    {
        currentPos = currentPos;
    }


    if (IsClickable && (IsLocalInsideBounds(originalPos) || IsLocalInsideBounds(currentPos)))
    {
        Vector2f dims = GetCollisionDimensions();
        Value = GetNewValue(currentPos);

        if (UsesTimeLerp() && CurrentTimeLerpSpeed <= 0.0f)
        {
            CurrentTimeLerpSpeed = TimeLerpSpeed;
            SetTimeLerp(0.0f);
        }
    }
}