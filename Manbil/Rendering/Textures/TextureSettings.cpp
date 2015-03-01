#include "TextureSettings.h"


std::string FilteringTypeToString(FilteringTypes filtType)
{
    switch (filtType)
    {
        case FT_NEAREST: return "Nearest";
        case FT_LINEAR: return "Linear";

        default:
            assert(false);
            return "Unknown filtering type: " + std::to_string(filtType);
    }
}
std::string WrappingTypeToString(WrappingTypes filtType)
{
    switch (filtType)
    {
        case WT_CLAMP: return "Clamp";
        case WT_WRAP: return "Wrap";

        default:
            assert(false);
            return "Unknown wrapping type: " + std::to_string(filtType);
    }
}

bool StringToFilteringType(const std::string & inStr, FilteringTypes & outVal)
{
    if (inStr.compare("Nearest") == 0)
    {
        outVal = FT_NEAREST;
        return true;
    }
    else if (inStr.compare("Linear") == 0)
    {
        outVal = FT_LINEAR;
        return true;
    }
    else return false;
}
bool StringToWrappingType(const std::string & inStr, WrappingTypes & outVal)
{
    if (inStr.compare("Clamp") == 0)
    {
        outVal = WT_CLAMP;
        return true;
    }
    else if (inStr.compare("Wrap") == 0)
    {
        outVal = WT_WRAP;
        return true;
    }
    else return false;
}

GLint ToGLInt(FilteringTypes tf, bool minFilter, bool genMips)
{
    return (tf == FilteringTypes::FT_LINEAR) ?
                ((minFilter && genMips) ?
                    GL_LINEAR_MIPMAP_LINEAR :
                    GL_LINEAR) :
                ((minFilter && genMips) ?
                    GL_NEAREST_MIPMAP_LINEAR :
                    GL_NEAREST);
}
GLint ToGLInt(WrappingTypes twa)
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
std::string ToString(PixelSizes size)
{
    switch (size)
    {
        case PS_8U: return "RGBA UInt 8";
        case PS_16U: return "RGBA UInt 16";
        case PS_16F: return "RGBA Float 16";
        case PS_32F: return "RGBA Float 32";
        case PS_8U_GREYSCALE: return "Greyscale UInt 8";
        case PS_16U_GREYSCALE: return "Greyscale UInt 16";
        case PS_32F_GREYSCALE: return "Greyscale Float 32";
        case PS_16U_DEPTH: return "Depth UInt 16";
        case PS_24U_DEPTH: return "Depth UInt 24";
        case PS_32F_DEPTH: return "Depth Float 32";

        default:
            assert(false);
            return "INVALID PIXEL SIZE";
    }
}
PixelSizes ToPixelSize(const std::string& sizeStr)
{
    if (sizeStr == std::string("RGBA UInt 8"))
    {
        return PS_8U;
    }
    else if (sizeStr == std::string("RGBA UInt 16"))
    {
        return PS_16U;
    }
    else if (sizeStr == std::string("RGBA Float 16"))
    {
        return PS_16F;
    }
    else if (sizeStr == std::string("RGBA Float 32"))
    {
        return PS_32F;
    }
    else if (sizeStr == std::string("Greyscale UInt 8"))
    {
        return PS_8U_GREYSCALE;
    }
    else if (sizeStr == std::string("Greyscale UInt 16"))
    {
        return PS_16U_GREYSCALE;
    }
    else if (sizeStr == std::string("Greyscale Float 32"))
    {
        return PS_32F_GREYSCALE;
    }
    else if (sizeStr == std::string("Depth UInt 16"))
    {
        return PS_16U_DEPTH;
    }
    else if (sizeStr == std::string("Depth UInt 24"))
    {
        return PS_24U_DEPTH;
    }
    else if (sizeStr == std::string("Depth Float 32"))
    {
        return PS_32F_DEPTH;
    }
    else
    {
        assert(false);
        return PS_8U;
    }
}


void TextureSampleSettings2D::ApplyMinFilter(bool usesMipmaps) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLInt(MinFilter, true, usesMipmaps));
}
//Sets the currently-bound 2D texture to use this setting's mag filter.
void TextureSampleSettings2D::ApplyMagFilter(bool usesMipmaps) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLInt(MagFilter, false, usesMipmaps));
}
//Sets the currently-bound 2D texture to use this setting's min and mag filters.
void TextureSampleSettings2D::ApplyFilter(bool usesMipmaps) const
{
    ApplyMinFilter(usesMipmaps);
    ApplyMagFilter(usesMipmaps);
}

//Sets the currently-bound 2D texture to use this setting's horizontal wrapping behavior.
void TextureSampleSettings2D::ApplyHorzWrapping(void) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLInt(HorzWrap));
}
//Sets the currently-bound 2D texture to use this setting's vertical wrapping behavior.
void TextureSampleSettings2D::ApplyVertWrapping(void) const
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLInt(VertWrap));
}
//Sets the currently-bound 2D texture to use this setting's horizontal/vertical wrapping behavior.
void TextureSampleSettings2D::ApplyWrapping(void) const
{
    ApplyHorzWrapping();
    ApplyVertWrapping();
}

//Applies all this instance's settings to the currently-bound 2D texture.
void TextureSampleSettings2D::ApplyAllSettings(bool usesMipmaps) const
{
    ApplyFilter(usesMipmaps);
    ApplyWrapping();
}


void TextureSampleSettings3D::ApplyMinFilter(TextureTypes type, bool usesMipmaps) const
{
    assert(type == TextureTypes::TT_3D || type == TextureTypes::TT_CUBE);
    glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_MIN_FILTER,
                    ToGLInt(MinFilter, true, usesMipmaps));
}
//Sets the currently-bound 3D/cubemap texture to use this setting's mag filter.
void TextureSampleSettings3D::ApplyMagFilter(TextureTypes type, bool usesMipmaps) const
{
    assert(type == TextureTypes::TT_3D || type == TextureTypes::TT_CUBE);
    glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_MAG_FILTER,
                    ToGLInt(MagFilter, false, usesMipmaps));
}
//Sets the currently-bound 3D/cubemap texture to use this setting's min and mag filters.
void TextureSampleSettings3D::ApplyFilter(TextureTypes type, bool usesMipmaps) const
{
    ApplyMinFilter(type, usesMipmaps);
    ApplyMagFilter(type, usesMipmaps);
}

//Sets the currently-bound 3D/cubemap texture to use this setting's X wrapping behavior.
void TextureSampleSettings3D::ApplyXWrapping(TextureTypes type) const
{
    assert(type == TextureTypes::TT_3D || type == TextureTypes::TT_CUBE);
    glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_WRAP_S, ToGLInt(XWrap));
}
//Sets the currently-bound 3D/cubemap texture to use this setting's Y wrapping behavior.
void TextureSampleSettings3D::ApplyYWrapping(TextureTypes type) const
{
    assert(type == TextureTypes::TT_3D || type == TextureTypes::TT_CUBE);
    glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_WRAP_T, ToGLInt(YWrap));
}
//Sets the currently-bound 3D/cubemap texture to use this setting's Y wrapping behavior.
void TextureSampleSettings3D::ApplyZWrapping(TextureTypes type) const
{
    assert(type == TextureTypes::TT_3D || type == TextureTypes::TT_CUBE);
    glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_WRAP_R, ToGLInt(ZWrap));
}
//Sets the currently-bound 3D/cubemap texture to use this setting's XYZ wrapping behavior.
void TextureSampleSettings3D::ApplyWrapping(TextureTypes type) const
{
    ApplyXWrapping(type);
    ApplyYWrapping(type);
    ApplyZWrapping(type);
}

//Applies all this instance's settings to the currently-bound 3D/cubemap texture.
void TextureSampleSettings3D::ApplyAllSettings(TextureTypes type, bool usesMipmaps) const
{
    ApplyFilter(type, usesMipmaps);
    ApplyWrapping(type);
}