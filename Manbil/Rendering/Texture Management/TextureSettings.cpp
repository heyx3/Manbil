#include "TextureSettings.h"


GLint TextureSampleSettings::ToGLInt(FilteringTypes tf, bool minFilter, bool genMips)
{
    return (tf == FilteringTypes::FT_LINEAR) ?
                ((minFilter && genMips) ?
                     GL_LINEAR_MIPMAP_LINEAR :
                     GL_LINEAR) :
                ((minFilter && genMips) ?
                     GL_NEAREST_MIPMAP_LINEAR :
                     GL_NEAREST);
}
GLint TextureSampleSettings::ToGLInt(WrappingTypes twa)
{
    return (twa == WrappingTypes::WT_CLAMP) ?
                GL_CLAMP_TO_EDGE :
                GL_REPEAT;
}



bool IsPixelSizeColor(PixelSizes pixelSize)
{
    switch (pixelSize)
    {
        case PS_8U:
        case PS_16U:
        case PS_16F:
        case PS_32F:
            return true;

        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            return false;

        default: assert(false); return false;
    }
}
bool IsPixelSizeGreyscale(PixelSizes pixelSize)
{
    switch (pixelSize)
    {
        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            return true;

        case PS_8U:
        case PS_16U:
        case PS_16F:
        case PS_32F:
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            return false;

        default: assert(false); return false;
    }
}
bool IsPixelSizeDepth(PixelSizes pixelSize)
{
    switch (pixelSize)
    {
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
        case PS_32F_DEPTH:
            return true;

        case PS_8U:
        case PS_16U:
        case PS_16F:
        case PS_32F:
        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_32F_GREYSCALE:
            return false;

        default: assert(false); return false;
    }
}
bool IsPixelSizeFloat(PixelSizes size)
{
    switch (size)
    {
        case PS_16F:
        case PS_32F:
        case PS_32F_GREYSCALE:
        case PS_32F_DEPTH:
            return true;

        case PS_8U:
        case PS_16U:
        case PS_8U_GREYSCALE:
        case PS_16U_GREYSCALE:
        case PS_16U_DEPTH:
        case PS_24U_DEPTH:
            return false;

        default: assert(false); return false;
    }
}
unsigned int GetComponentSize(PixelSizes size)
{
    switch (size)
    {
        case PS_8U:
        case PS_8U_GREYSCALE:
            return 8;

        case PS_16F:
        case PS_16U:
        case PS_16U_GREYSCALE:
        case PS_16U_DEPTH:
            return 16;

        case PS_24U_DEPTH:
            return 24;

        case PS_32F:
        case PS_32F_GREYSCALE:
        case PS_32F_DEPTH:
            return 32;

        default: assert(false); return 0;
    }
}
GLenum ToGLenum(PixelSizes pixelSize)
{
    switch (pixelSize)
    {
        case PS_8U: return GL_RGBA8;
        case PS_16U: return GL_RGBA16;
        case PS_16F: return GL_RGBA16F;
        case PS_32F: return GL_RGBA32F;
        case PS_8U_GREYSCALE: return GL_R8;
        case PS_16U_GREYSCALE: return GL_R16;
        case PS_32F_GREYSCALE: return GL_R32F;
        case PS_16U_DEPTH: return GL_DEPTH_COMPONENT16;
        case PS_24U_DEPTH: return GL_DEPTH_COMPONENT24;
        case PS_32F_DEPTH: return GL_DEPTH_COMPONENT32F;

        default: assert(false); return GL_INVALID_ENUM;
    }
}