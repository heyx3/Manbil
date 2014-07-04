#include "TextureSettings.h"


GLint TextureSettings::ToGLInt(FilteringTypes tf, bool minFilter, bool genMips)
{
    return (tf == FilteringTypes::FT_LINEAR) ?
                ((minFilter && genMips) ?
                     GL_LINEAR_MIPMAP_LINEAR :
                     GL_LINEAR) :
                ((minFilter && genMips) ?
                     GL_NEAREST_MIPMAP_LINEAR :
                     GL_NEAREST);
}
GLint TextureSettings::ToGLInt(WrappingTypes twa)
{
    return (twa == WrappingTypes::WT_CLAMP) ?
                GL_CLAMP_TO_EDGE :
                GL_REPEAT;
}


GLenum ColorTextureSettings::ToInternalFormat(PixelSizes size)
{
    switch (size)
    {
        case PixelSizes::CTS_8: return GL_RGBA8;
        case PixelSizes::CTS_16: return GL_RGBA16;
        case PixelSizes::CTS_32: return GL_RGBA32F;
        case PixelSizes::CTS_8_GREYSCALE: return GL_R8;
        case PixelSizes::CTS_16_GREYSCALE: return GL_R16;
        default: return GL_INVALID_ENUM;
    }
}
std::string ColorTextureSettings::ToString(PixelSizes size)
{
    switch (size)
    {
        case PixelSizes::CTS_8: return "8";
        case PixelSizes::CTS_16: return "16";
        case PixelSizes::CTS_32: return "32";
        case PixelSizes::CTS_8_GREYSCALE: return "8_greyscale";
        case PixelSizes::CTS_16_GREYSCALE: return "16_greyscale";
        default: return "UNKNOWN_SIZE";
    }
}


std::string DepthTextureSettings::ToString(PixelSizes size)
{
    switch (size)
    {
        case PixelSizes::DTS_16: return "16";
        case PixelSizes::DTS_24: return "24";
        case PixelSizes::DTS_32: return "32";
        default: return "UNKNOWN_SIZE";
    }
}
GLenum DepthTextureSettings::ToEnum(PixelSizes size)
{
    switch (size)
    {
        case PixelSizes::DTS_16: return GL_DEPTH_COMPONENT16;
        case PixelSizes::DTS_24: return GL_DEPTH_COMPONENT24;
        case PixelSizes::DTS_32: return GL_DEPTH_COMPONENT32;
        default: return GL_INVALID_ENUM;
    }
}