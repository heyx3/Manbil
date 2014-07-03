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
void TextureConverters::ToTexture(const Array2D<Vector4b> & inColor, const ColorTextureSettings & settings, RenderObjHandle outTex)
{
    RenderDataHandler::SetTexture2DDataUBytes(outTex, settings, (unsigned char*)inColor.GetArray());
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
    Array2D<Vector4b> valueBytes(0, 0);
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
    Array2D<Vector4b> valueBytes(0, 0);
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
void TextureConverters::ToTexture(const Array2D<Vector4f> & inColor, const ColorTextureSettings & settings, RenderObjHandle outTex)
{
    RenderDataHandler::SetTexture2DDataFloats(outTex, settings, &inColor.GetArray()[0][0]);
}














/*
const float TextureConverters::ByteToFloatScalar = 1.0f / 255.0f;

void TextureConverters::ToArray(const sf::Image & inImg, ChannelsIn channel, Array2D<UByte> & channelOut)
{
    switch (channel)
    {
    case ChannelsIn::CI_Red:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return col.r; });
        break;

    case ChannelsIn::CI_Green:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return col.g; });
        break;

    case ChannelsIn::CI_Blue:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return col.b; });
        break;

    case ChannelsIn::CI_Alpha:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return col.a; });
        break;

    case ChannelsIn::CI_GrayscaleMax:
        WriteArray<UByte>(inImg, channelOut,
                          [](sf::Color col)
                          {
                              return BasicMath::Max(col.r, BasicMath::Max(col.g, col.b));
                          });
        break;

    case ChannelsIn::CI_GrayscaleMin:
        WriteArray<UByte>(inImg, channelOut,
                          [](sf::Color col)
        {
            return BasicMath::Min(col.r, BasicMath::Min(col.g, col.b));
        });
        break;

    case ChannelsIn::CI_GrayscaleAverage:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return (UByte)((col.r + col.g + col.b) / 3); });
        break;

    default: assert(false);
    }
}
void TextureConverters::ToArray(const sf::Image & inImg, ChannelsIn channel, Array2D<float> & channelOut)
{
    switch (channel)
    {
    case ChannelsIn::CI_Red:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return ByteToFloatScalar * col.r; });
        break;

    case ChannelsIn::CI_Green:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return ByteToFloatScalar * col.g; });
        break;

    case ChannelsIn::CI_Blue:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return ByteToFloatScalar * col.b; });
        break;

    case ChannelsIn::CI_Alpha:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return ByteToFloatScalar * col.a; });
        break;

    case ChannelsIn::CI_GrayscaleMax:
        WriteArray<float>(inImg, channelOut,
                          [](sf::Color col)
        {
            return ByteToFloatScalar * BasicMath::Max(col.r, BasicMath::Max(col.g, col.b));
        });
        break;

    case ChannelsIn::CI_GrayscaleMin:
        WriteArray<float>(inImg, channelOut,
                          [](sf::Color col)
        {
            return ByteToFloatScalar * BasicMath::Min(col.r, BasicMath::Min(col.g, col.b));
        });
        break;

    case ChannelsIn::CI_GrayscaleAverage:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return (col.r + col.g + col.b) / 3.0f; });
        break;

    default: assert(false);
    }
}

void TextureConverters::ToImage(const Array2D<UByte> & channelIn, ChannelsOut channel, sf::Image & outImg, UByte defaultColor, UByte defaultAlpha)
{
    UByte values[] = { defaultColor, defaultAlpha };

    switch (channel)
    {
    case ChannelsOut::CO_Red:
        WriteImg<UByte>(channelIn, outImg, &values,
                        [](void* df, UByte value)
                        {
                            UByte * vs = (UByte*)df;
                            return sf::Color(value, vs[0], vs[0], vs[1]);
                        });
        break;

    case ChannelsOut::CO_Green:
        WriteImg<UByte>(channelIn, outImg, &values,
                        [](void* df, UByte value)
                        {
                            UByte * vs = (UByte*)df;
                            return sf::Color(vs[0], value, vs[0], vs[1]);
                        });
        break;

    case ChannelsOut::CO_Blue:
        WriteImg<UByte>(channelIn, outImg, &values,
                        [](void* df, UByte value)
                        {
                            UByte * vs = (UByte*)df;
                            return sf::Color(vs[0], vs[0], value, vs[1]);
                        });
        break;

    case ChannelsOut::CO_Alpha:
        WriteImg<UByte>(channelIn, outImg, &values,
                        [](void* df, UByte value)
                        {
                            UByte * vs = (UByte*)df;
                            return sf::Color(vs[0], vs[0], vs[0], value);
                        });
        break;

    case ChannelsOut::CO_AllColorChannels:
        WriteImg<UByte>(channelIn, outImg, &defaultAlpha,
                        [](void* df, UByte value)
                        {
                            UByte ds = *(UByte*)df;
                            return sf::Color(value, value, value, ds);
                        });
        break;

    case ChannelsOut::CO_AllChannels:
        WriteImg<UByte>(channelIn, outImg, 0,
                        [](void* df, UByte value)
                        {
                            return sf::Color(value, value, value, value);
                        });
        break;
        
    default: assert(false);
    }
}
void TextureConverters::ToImage(const Array2D<float> & channelIn, ChannelsOut channel, sf::Image & outImg, float defaultColor, float defaultAlpha)
{
    UByte value1 = (UByte)(255.0f * defaultColor),
          value2 = (UByte)(255.0f * defaultAlpha),
          values[] = { value1, value2 };

    switch (channel)
    {
    case ChannelsOut::CO_Red:
        WriteImg<float>(channelIn, outImg, &values,
                        [](void* df, float value)
        {
            UByte * vs = (UByte*)df;
            return sf::Color((UByte)(255.0f * value), vs[0], vs[0], vs[1]);
        });
        break;

    case ChannelsOut::CO_Green:
        WriteImg<float>(channelIn, outImg, &values,
                        [](void* df, float value)
        {
            UByte * vs = (UByte*)df;
            return sf::Color(vs[0], (UByte)(255.0f * value), vs[0], vs[1]);
        });
        break;

    case ChannelsOut::CO_Blue:
        WriteImg<float>(channelIn, outImg, &values,
                        [](void* df, float value)
        {
            UByte * vs = (UByte*)df;
            return sf::Color(vs[0], vs[0], (UByte)(255.0f * value), vs[1]);
        });
        break;

    case ChannelsOut::CO_Alpha:
        WriteImg<float>(channelIn, outImg, &values,
                        [](void* df, float value)
        {
            UByte * vs = (UByte*)df;
            return sf::Color(vs[0], vs[0], vs[0], (UByte)(255.0f * value));
        });
        break;

    case ChannelsOut::CO_AllColorChannels:
        WriteImg<float>(channelIn, outImg, &defaultAlpha,
                        [](void* df, float value)
        {
            UByte ds = *(UByte*)df;
            UByte val = (UByte)(255.0f * value);
            return sf::Color(val, val, val, ds);
        });
        break;

    case ChannelsOut::CO_AllChannels:
        WriteImg<float>(channelIn, outImg, 0,
                        [](void* df, float value)
        {
            UByte val = (UByte)(255.0f * value);
            return sf::Color(val, val, val, val);
        });
        break;

    default: assert(false);
    }
}

#pragma warning(default: 4100)

*/