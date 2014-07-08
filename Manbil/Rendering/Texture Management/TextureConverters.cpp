#include "TextureConverters.h"

#include <assert.h>

#pragma warning(disable: 4100)


void TextureConverters::ToArray(const sf::Image & inImg, Array2D<Vector4b> & colorOut)
{
    colorOut.Reset(inImg.getSize().x, inImg.getSize().y);

    for (Vector2i loc; loc.y < colorOut.GetHeight(); ++loc.y)
        for (loc.x = 0; loc.x < colorOut.GetWidth(); ++loc.x)
            colorOut[loc] = ToVectorB(inImg.getPixel((unsigned int)loc.x, (unsigned int)loc.y));
}
void TextureConverters::ToArray(const sf::Texture & inTex, Array2D<Vector4b> & colorOut)
{
    return ToArray(inTex.copyToImage(), colorOut);
}
void TextureConverters::ToArray(RenderObjHandle inTex, Array2D<Vector4b> & colorOut)
{
    RenderDataHandler::GetTexture2DData(inTex, Vector2i((int)colorOut.GetWidth(), (int)colorOut.GetHeight()), colorOut);
}

void TextureConverters::ToImage(const Array2D<Vector4b> & inColor, sf::Image & outImg)
{
    outImg.create(inColor.GetWidth(), inColor.GetHeight(), (sf::Uint8*)inColor.GetArray());
}
void TextureConverters::ToTexture(const Array2D<Vector4b> & inColor, sf::Texture & outTex)
{
    outTex.update((sf::Uint8*)inColor.GetArray());
}
void TextureConverters::ToTexture(const Array2D<Vector4b> & inColor, ColorTextureSettings settings, RenderObjHandle outTex)
{
    RenderDataHandler::BindTexture(TextureTypes::TT_2D, outTex);
    settings.Width = inColor.GetWidth();
    settings.Height = inColor.GetHeight();
    RenderDataHandler::SetTextureFromBytes(settings, &inColor.GetArray()[0].x);
}


void TextureConverters::ToArray(const sf::Image & inImg, Array2D<Vector4f> & colorOut)
{
    colorOut.Reset(inImg.getSize().x, inImg.getSize().y);

    for (Vector2i loc; loc.y < colorOut.GetHeight(); ++loc.y)
        for (loc.x = 0; loc.x < colorOut.GetWidth(); ++loc.x)
            colorOut[loc] = ToVectorF(inImg.getPixel((unsigned int)loc.x, (unsigned int)loc.y));
}
void TextureConverters::ToArray(const sf::Texture & inTex, Array2D<Vector4f> & colorOut)
{
    return ToArray(inTex.copyToImage(), colorOut);
}
void TextureConverters::ToArray(RenderObjHandle inTex, Array2D<Vector4f> & colorOut)
{
    RenderDataHandler::GetTexture2DData(inTex, Vector2i((int)colorOut.GetWidth(), (int)colorOut.GetHeight()), colorOut);
}

void TextureConverters::ToImage(const Array2D<Vector4f> & inColor, sf::Image & outImg)
{
    Array2D<Vector4b> valueBytes(inColor.GetWidth(), inColor.GetHeight());
    valueBytes.Fill([&inColor](Vector2i loc, Vector4b * outVal)
    {
        Vector4f col = inColor[loc];
        *outVal = Vector4b((unsigned char)BasicMath::RoundToInt(col.x * 255.0f),
                           (unsigned char)BasicMath::RoundToInt(col.y * 255.0f),
                           (unsigned char)BasicMath::RoundToInt(col.z * 255.0f),
                           (unsigned char)BasicMath::RoundToInt(col.w * 255.0f));
    });
    outImg.create(inColor.GetWidth(), inColor.GetHeight(), (sf::Uint8*)valueBytes.GetArray());
}
void TextureConverters::ToTexture(const Array2D<Vector4f> & inColor, sf::Texture & outTex)
{
    Array2D<Vector4b> valueBytes(inColor.GetWidth(), inColor.GetHeight());
    valueBytes.Fill([&inColor](Vector2i loc, Vector4b * outVal)
    {
        Vector4f col = inColor[loc];
        *outVal = Vector4b((unsigned char)BasicMath::RoundToInt(col.x * 255.0f),
                           (unsigned char)BasicMath::RoundToInt(col.y * 255.0f),
                           (unsigned char)BasicMath::RoundToInt(col.z * 255.0f),
                           (unsigned char)BasicMath::RoundToInt(col.w * 255.0f));
    });
    outTex.update((sf::Uint8*)valueBytes.GetArray());
}
void TextureConverters::ToTexture(const Array2D<Vector4f> & inColor, ColorTextureSettings settings, RenderObjHandle outTex)
{
    RenderDataHandler::BindTexture(TextureTypes::TT_2D, outTex);
    settings.Width = inColor.GetWidth();
    settings.Height = inColor.GetHeight();
    RenderDataHandler::SetTextureFromFloats(settings, &inColor.GetArray()[0].x);
}


#pragma warning(default: 4100)