#include "TextureConverters.h"

#include <assert.h>

const float TextureConverters::ByteToFloatScalar = 1.0f / 255.0f;

void TextureConverters::ToArray(const sf::Image & inImg, Channels channel, Fake2DArray<UByte> & channelOut)
{
    switch (channel)
    {
    case Channels::Red:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return col.r; });
        break;

    case Channels::Green:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return col.g; });
        break;

    case Channels::Blue:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return col.b; });
        break;

    case Channels::Alpha:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return col.a; });
        break;

    case Channels::GrayscaleMax:
        WriteArray<UByte>(inImg, channelOut,
                          [](sf::Color col)
                          {
                              return BasicMath::Max(col.r, BasicMath::Max(col.g, col.b));
                          });
        break;

    case Channels::GrayscaleMin:
        WriteArray<UByte>(inImg, channelOut,
                          [](sf::Color col)
        {
            return BasicMath::Min(col.r, BasicMath::Min(col.g, col.b));
        });
        break;

    case Channels::GrayscaleAverage:
        WriteArray<UByte>(inImg, channelOut, [](sf::Color col) { return (UByte)((col.r + col.g + col.b) / 3); });
        break;

    default: assert(false);
    }
}
void TextureConverters::ToArray(const sf::Image & inImg, Channels channel, Fake2DArray<float> & channelOut)
{
    switch (channel)
    {
    case Channels::Red:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return ByteToFloatScalar * col.r; });
        break;

    case Channels::Green:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return ByteToFloatScalar * col.g; });
        break;

    case Channels::Blue:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return ByteToFloatScalar * col.b; });
        break;

    case Channels::Alpha:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return ByteToFloatScalar * col.a; });
        break;

    case Channels::GrayscaleMax:
        WriteArray<float>(inImg, channelOut,
                          [](sf::Color col)
        {
            return ByteToFloatScalar * BasicMath::Max(col.r, BasicMath::Max(col.g, col.b));
        });
        break;

    case Channels::GrayscaleMin:
        WriteArray<float>(inImg, channelOut,
                          [](sf::Color col)
        {
            return ByteToFloatScalar * BasicMath::Min(col.r, BasicMath::Min(col.g, col.b));
        });
        break;

    case Channels::GrayscaleAverage:
        WriteArray<float>(inImg, channelOut, [](sf::Color col) { return (col.r + col.g + col.b) / 3.0f; });
        break;

    default: assert(false);
    }
}

void TextureConverters::ToImage(const Fake2DArray<UByte> & channelIn, Channels channel, sf::Image & outImg, UByte defaultColor, UByte defaultAlpha)
{
    UByte values[] = { defaultColor, defaultAlpha };

    switch (channel)
    {
    case Channels::Red:
        WriteImg<UByte>(channelIn, outImg, &values,
                        [](void* df, UByte value)
                        {
                            UByte * vs = (UByte*)df;
                            return sf::Color(value, vs[0], vs[0], vs[1]);
                        });
        break;

    case Channels::Green:
        WriteImg<UByte>(channelIn, outImg, &values,
                        [](void* df, UByte value)
                        {
                            UByte * vs = (UByte*)df;
                            return sf::Color(vs[0], value, vs[0], vs[1]);
                        });
        break;

    case Channels::Blue:
        WriteImg<UByte>(channelIn, outImg, &values,
                        [](void* df, UByte value)
                        {
                            UByte * vs = (UByte*)df;
                            return sf::Color(vs[0], vs[0], value, vs[1]);
                        });
        break;

    case Channels::Alpha:
        WriteImg<UByte>(channelIn, outImg, &values,
                        [](void* df, UByte value)
                        {
                            UByte * vs = (UByte*)df;
                            return sf::Color(vs[0], vs[0], vs[0], value);
                        });
        break;

    case Channels::AllColorChannels:
        WriteImg<UByte>(channelIn, outImg, &defaultAlpha,
                        [](void* df, UByte value)
                        {
                            UByte ds = *(UByte*)df;
                            return sf::Color(value, value, value, ds);
                        });
        break;

    case Channels::AllChannels:
        WriteImg<UByte>(channelIn, outImg, 0,
                        [](void* df, UByte value)
                        {
                            return sf::Color(value, value, value, value);
                        });
        break;

    default: assert(false);
    }
}
void TextureConverters::ToImage(const Fake2DArray<float> & channelIn, Channels channel, sf::Image & outImg, float defaultColor, float defaultAlpha)
{
    UByte value1 = (UByte)(255.0f * defaultColor),
          value2 = (UByte)(255.0f * defaultAlpha),
          values[] = { value1, value2 };

    switch (channel)
    {
    case Channels::Red:
        WriteImg<float>(channelIn, outImg, &values,
                        [](void* df, float value)
        {
            UByte * vs = (UByte*)df;
            return sf::Color((UByte)(255.0f * value), vs[0], vs[0], vs[1]);
        });
        break;

    case Channels::Green:
        WriteImg<float>(channelIn, outImg, &values,
                        [](void* df, float value)
        {
            UByte * vs = (UByte*)df;
            return sf::Color(vs[0], (UByte)(255.0f * value), vs[0], vs[1]);
        });
        break;

    case Channels::Blue:
        WriteImg<float>(channelIn, outImg, &values,
                        [](void* df, float value)
        {
            UByte * vs = (UByte*)df;
            return sf::Color(vs[0], vs[0], (UByte)(255.0f * value), vs[1]);
        });
        break;

    case Channels::Alpha:
        WriteImg<float>(channelIn, outImg, &values,
                        [](void* df, float value)
        {
            UByte * vs = (UByte*)df;
            return sf::Color(vs[0], vs[0], vs[0], (UByte)(255.0f * value));
        });
        break;

    case Channels::AllColorChannels:
        WriteImg<float>(channelIn, outImg, &defaultAlpha,
                        [](void* df, float value)
        {
            UByte ds = *(UByte*)df;
            UByte val = (UByte)(255.0f * value);
            return sf::Color(val, val, val, ds);
        });
        break;

    case Channels::AllChannels:
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