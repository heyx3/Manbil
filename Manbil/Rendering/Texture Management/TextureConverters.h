#pragma once

#include <SFML/Graphics/Texture.hpp>
#include "../../Math/Array2D.h"
#include "TextureChannels.h"
#include "../../RenderDataHandler.h"


#pragma warning(disable: 4100)


//Converts between sf::Images/sf::Textures/(OpenGL textures) and arrays.
class TextureConverters
{
public:

    static void ToArray(const sf::Image & inImg, Array2D<Vector4b> & colorOut);
    static void ToArray(const sf::Texture & inTex, Array2D<Vector4b> & colorOut);
    static void ToArray(RenderObjHandle inTex, Array2D<Vector4b> & colorOut);

    static void ToImage(const Array2D<Vector4b> & inColor, sf::Image & outImg);
    static void ToTexture(const Array2D<Vector4b> & inColor, sf::Texture & outTex);
    static void ToTexture(const Array2D<Vector4b> & inColor, ColorTextureSettings settings, RenderObjHandle outTex);


    static void ToArray(const sf::Image & inImg, Array2D<Vector4f> & colorOut);
    static void ToArray(const sf::Texture & inTex, Array2D<Vector4f> & colorOut);
    static void ToArray(RenderObjHandle inTex, Array2D<Vector4f> & colorOut);

    static void ToImage(const Array2D<Vector4f> & inColor, sf::Image & outImg);
    static void ToTexture(const Array2D<Vector4f> & inColor, sf::Texture & outTex);
    static void ToTexture(const Array2D<Vector4f> & inColor, ColorTextureSettings settings, RenderObjHandle outTex);

private:

    static Vector4b ToVectorB(sf::Color inCol) { return Vector4b(inCol.r, inCol.g, inCol.b, inCol.a); }
    static sf::Color ToColor(Vector4b inCol) { return sf::Color(inCol.x, inCol.y, inCol.z, inCol.w); }

    static Vector4f ToVectorF(sf::Color inCol)
    {
        const float divideBy255 = 1.0f / 255.0f;
        return Vector4f(inCol.r * divideBy255, inCol.g * divideBy255, inCol.b * divideBy255, inCol.a * divideBy255);
    }
    static sf::Color ToColor(Vector4f inCol)
    {
        return sf::Color((unsigned char)BasicMath::Clamp<int>((int)((float)inCol.x * 255.0f), 0, 255),
                         (unsigned char)BasicMath::Clamp<int>((int)((float)inCol.y * 255.0f), 0, 255),
                         (unsigned char)BasicMath::Clamp<int>((int)((float)inCol.z * 255.0f), 0, 255),
                         (unsigned char)BasicMath::Clamp<int>((int)((float)inCol.w * 255.0f), 0, 255));
    }
};


#pragma warning(default: 4100)