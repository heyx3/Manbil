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
    static void ToTexture(const Array2D<Vector4b> & inColor, const ColorTextureSettings & settings, RenderObjHandle outTex);


    static void ToArray(const sf::Image & inImg, Array2D<Vector4f> & colorOut);
    static void ToArray(const sf::Texture & inTex, Array2D<Vector4f> & colorOut);
    static void ToArray(RenderObjHandle inTex, Array2D<Vector4f> & colorOut);

    static void ToImage(const Array2D<Vector4f> & inColor, sf::Image & outImg);
    static void ToTexture(const Array2D<Vector4f> & inColor, sf::Texture & outTex);
    static void ToTexture(const Array2D<Vector4f> & inColor, const ColorTextureSettings & settings, RenderObjHandle outTex);

private:

    static Vector4b ToVectorB(sf::Color inCol) { return Vector4b(inCol.r, inCol.g, inCol.b, inCol.a); }
    static sf::Color ToColor(Vector4b inCol) { return sf::Color(inCol.x, inCol.y, inCol.z, inCol.w); }

    static Vector4f ToVectorF(sf::Color inCol)
    {
        const float divideBy255 = 1.0f / 255.0f;
        return Vector4f(inCol.r * divideBy255, inCol.g * divideBy255, inCol.b * divideBy255, inCol.a * divideBy255);
    }
    static sf::Color ToColor(Vector4f inCol) { return sf::Color(inCol.x, inCol.y, inCol.z, inCol.w); }
};


/*
//Helps convert between SFML images and 2d arrays.
class TextureConvertersSFML
{
public:
    
    typedef unsigned char UByte;

    static void ToArray(const sf::Image & inImg, Array2D<Vector3b> & colorOut) { WriteArray<Vector3b>(inImg, colorOut, [](sf::Color col) { return Vector3b(col.r, col.g, col.b); }); }
    static void ToArray(const sf::Image & inImg, Array2D<Vector4b> & textureOut) { WriteArray<Vector4b>(inImg, textureOut, [](sf::Color col) { return Vector4b(col.r, col.g, col.b, col.a); }); }
    static void ToArray(const sf::Image & inImg, Array2D<Vector3f> & colorOut) { WriteArray<Vector3f>(inImg, colorOut, [](sf::Color col) { return Vector3f(col.r, col.g, col.b) * ByteToFloatScalar; }); }
    static void ToArray(const sf::Image & inImg, Array2D<Vector4f> & textureOut) { WriteArray<Vector4f>(inImg, textureOut, [](sf::Color col) { return Vector4f(col.r, col.g, col.b, col.a) * ByteToFloatScalar; }); }
    static void ToArray(const sf::Image & inImg, ChannelsIn channel, Array2D<UByte> & channelValues);
    static void ToArray(const sf::Image & inImg, ChannelsIn channel, Array2D<float> & channelValues);

    template<typename ArrayType>
    static void ToArray(const sf::Image & img, Array2D<ArrayType> & outData, ArrayType (*ColorToData)(sf::Color imgColor))
    {
        return WriteArray(img, outData, ColorToData);
    }


    static void ToImage(const Array2D<Vector3b> & colorIn, sf::Image & outImg, UByte alpha = 255) { WriteImg<Vector3b>(colorIn, outImg, &alpha, [](void* al, Vector3b dat) { return sf::Color(dat.x, dat.y, dat.z, *(UByte*) al); }); }
    static void ToImage(const Array2D<Vector4b> & textureIn, sf::Image & outImg) { WriteImg<Vector4b>(textureIn, outImg, 0, [](void* al, Vector4b dat) { return sf::Color(dat.x, dat.y, dat.z, dat.w); }); }
    static void ToImage(const Array2D<Vector3f> & colorIn, sf::Image & outImg, float alpha = 1.0f) { UByte al = (UByte)(255.0f * alpha); WriteImg<Vector3f>(colorIn, outImg, &al, [](void* aa, Vector3f dat) { dat *= 255.0f; return sf::Color((UByte)dat.x, (UByte)dat.y, (UByte)dat.z, *(UByte*)aa); }); }
    static void ToImage(const Array2D<Vector4f> & textureIn, sf::Image & outImg) { WriteImg<Vector4f>(textureIn, outImg, 0, [](void* p, Vector4f dat) { dat *= 255.0f; return sf::Color((UByte)dat.x, (UByte)dat.y, (UByte)dat.z, (UByte)dat.w); }); }
    static void ToImage(const Array2D<UByte> & channelIn, ChannelsOut channel, sf::Image & finalImg, UByte defaultColor = 0, UByte defaultAlpha = 0);
    static void ToImage(const Array2D<float> & channelIn, ChannelsOut channel, sf::Image & finalImg, float defaultColor = 0.0f, float defaultAlpha = 0.0f);

    template<typename ArrayType>
    static void ToImage(const Array2D<ArrayType> & inData, sf::Image & outImg, void* pData, sf::Color (*DataToColor)(void* pData, ArrayType imgDataElement))
    {
        WriteImg(inData, outImg, pData, DataToColor);
    }

private:

    static const float ByteToFloatScalar;

    template<typename ArrayType>
    static void WriteArray(const sf::Image & img, Array2D<ArrayType> & outData, ArrayType (*GetArrayElement)(sf::Color texColor))
    {
        for (unsigned int x = 0; x < outData.GetWidth(); ++x)
        {
            for (unsigned int y = 0; y < outData.GetHeight(); ++y)
            {
                outData[Vector2i(x, y)] = GetArrayElement(img.getPixel(x, y));
            }
        }
    }
    template<typename ArrayType>
    static void WriteImg(const Array2D<ArrayType> & imgData, sf::Image & img, void* data, sf::Color (*GetImgColor)(void* pData, ArrayType imgDataElement))
    {
        img.create(imgData.GetWidth(), imgData.GetHeight());

        for (unsigned int x = 0; x < imgData.GetWidth(); ++x)
        {
            for (unsigned int y = 0; y < imgData.GetHeight(); ++y)
            {
                img.setPixel(x, y, GetImgColor(data, imgData[Vector2i(x, y)]));
            }
        }
    }
};
*/

#pragma warning(default: 4100)