#include "TextureConverters.h"

#include <assert.h>

#pragma warning(disable: 4100)


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