#include "TextureSettings.h"


GLint TextureSettings::ToGLInt(FilteringTypes tf, bool minFilter, bool genMips)
{
    return (tf == FilteringTypes::MTF_LINEAR) ?
                ((minFilter && genMips) ?
                     GL_LINEAR_MIPMAP_LINEAR :
                     GL_LINEAR) :
                GL_NEAREST;
}
GLint TextureSettings::ToGLInt(WrappingTypes twa)
{
    return (twa == WrappingTypes::MTF_CLAMP) ?
                GL_CLAMP_TO_EDGE :
                GL_REPEAT;
}


GLenum ColorTextureSettings::ToInternalFormat(Sizes size)
{
    switch (size)
    {
        case Sizes::CTS_8: return GL_RGBA8;
        case Sizes::CTS_16: return GL_RGBA16;
        case Sizes::CTS_32: return GL_RGBA32F;
        case Sizes::CTS_8_GREYSCALE: return GL_R8;
        case Sizes::CTS_16_GREYSCALE: return GL_R16;
        default: return GL_INVALID_ENUM;
    }
}
GLenum ColorTextureSettings::ToFormat(Sizes size)
{
    switch (size)
    {
        case Sizes::CTS_8:
        case Sizes::CTS_16:
        case Sizes::CTS_32:
            return GL_RGBA;
        case Sizes::CTS_8_GREYSCALE:
        case Sizes::CTS_16_GREYSCALE:
            return GL_RED;
        default: return GL_INVALID_ENUM;
    }
}
std::string ColorTextureSettings::ToString(Sizes size)
{
    switch (size)
    {
        case Sizes::CTS_8: return "8";
        case Sizes::CTS_16: return "16";
        case Sizes::CTS_32: return "32";
        case Sizes::CTS_8_GREYSCALE: return "8_greyscale";
        case Sizes::CTS_16_GREYSCALE: return "16_greyscale";
        default: return "UNKNOWN_SIZE";
    }
}


std::string DepthTextureSettings::ToString(Sizes size)
{
    switch (size)
    {
        case Sizes::DTS_16: return "16";
        case Sizes::DTS_24: return "24";
        case Sizes::DTS_32: return "32";
        default: return "UNKNOWN_SIZE";
    }
}
GLenum DepthTextureSettings::ToEnum(Sizes size)
{
    switch (size)
    {
        case Sizes::DTS_16: return GL_DEPTH_COMPONENT16;
        case Sizes::DTS_24: return GL_DEPTH_COMPONENT24;
        case Sizes::DTS_32: return GL_DEPTH_COMPONENT32;
        default: return GL_INVALID_ENUM;
    }
}