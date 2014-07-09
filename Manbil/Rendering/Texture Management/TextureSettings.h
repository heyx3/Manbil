#pragma once

#include "../../OpenGLIncludes.h"
#include <assert.h>


//Handles texture settings.
struct TextureSettings
{
public:

    //The types of texture filtering.
    enum FilteringTypes
    {
        FT_NEAREST,
        FT_LINEAR,
    };

    //The types of texture wrapping.
    enum WrappingTypes
    {
        WT_CLAMP,
        WT_WRAP,
    };


    FilteringTypes MinFilter, MagFilter;
    WrappingTypes HorzWrap, VertWrap;
    TextureTypes TextureType; //TODO: Instead of holding a texture type, take one in the "Apply" functions themselves. Remove the "Cubemap" from the end of all functions.


    TextureSettings(FilteringTypes min, FilteringTypes mag, WrappingTypes horz, WrappingTypes vert, TextureTypes texType = TextureTypes::TT_2D)
        : MinFilter(min), MagFilter(mag), HorzWrap(horz), VertWrap(vert), TextureType(texType)
    {
        assert(texType == TextureTypes::TT_2D || texType == TextureTypes::TT_CUBE);
    }
    TextureSettings(FilteringTypes filter = FilteringTypes::FT_NEAREST, WrappingTypes wrap = WrappingTypes::WT_WRAP, TextureTypes texType = TextureTypes::TT_2D)
        : MinFilter(filter), MagFilter(filter), HorzWrap(wrap), VertWrap(wrap), TextureType(texType)
    {
        assert(texType == TextureTypes::TT_2D || texType == TextureTypes::TT_CUBE);
    }


    //Sets the currently-bound texture to use this setting's min filter.
    void ApplyMinFilter(bool usesMipmaps) const
    {
        glTexParameteri(TextureTypeToGLEnum(TextureType), GL_TEXTURE_MIN_FILTER, ToGLInt(MinFilter, true, usesMipmaps));
    }
    //Sets the currently-bound texture to use this setting's mag filter.
    void ApplyMagFilter(bool usesMipmaps) const
    {
        glTexParameteri(TextureTypeToGLEnum(TextureType), GL_TEXTURE_MAG_FILTER, ToGLInt(MagFilter, false, usesMipmaps));
    }
    //Sets the currently-bound texture to use this setting's min and mag filters.
    void ApplyFilter(bool usesMipmaps) const
    {
        ApplyMinFilter(usesMipmaps);
        ApplyMagFilter(usesMipmaps);
    }

    //Sets the currently-bound texture to use this setting's horizontal wrapping behavior.
    void ApplyHorzWrapping(void) const
    {
        glTexParameteri(TextureTypeToGLEnum(TextureType), GL_TEXTURE_WRAP_S, ToGLInt(HorzWrap));
    }
    //Sets the currently-bound texture to use this setting's vertical wrapping behavior.
    void ApplyVertWrapping(void) const
    {
        glTexParameteri(TextureTypeToGLEnum(TextureType), GL_TEXTURE_WRAP_T, ToGLInt(VertWrap));
    }
    //Sets the currently-bound texture to use this setting's horizontal/vertical wrapping behavior.
    void ApplyWrapping(void) const
    {
        ApplyHorzWrapping();
        ApplyVertWrapping();
    }

    //Applies all this instance's settings to the currently-bound texture.
    void ApplyAllSettings(bool usesMipmaps) const
    {
        ApplyFilter(usesMipmaps);
        ApplyWrapping();
    }


    //Sets the currently-bound cubemap texture to use this setting's min filter.
    void ApplyMinFilterCubemap(CubeTextureTypes cubemapFace, bool usesMipmaps) const
    {
        glTexParameteri(TextureTypeToGLEnum(cubemapFace), GL_TEXTURE_MIN_FILTER, ToGLInt(MinFilter, true, usesMipmaps));
    }
    //Sets the currently-bound cubemap texture to use this setting's mag filter.
    void ApplyMagFilterCubemap(CubeTextureTypes cubemapFace, bool usesMipmaps) const
    {
        glTexParameteri(TextureTypeToGLEnum(cubemapFace), GL_TEXTURE_MAG_FILTER, ToGLInt(MagFilter, false, usesMipmaps));
    }
    //Sets the currently-bound cubemap texture to use this setting's min and mag filters.
    void ApplyFilterCubemap(CubeTextureTypes cubemapFace, bool usesMipmaps) const
    {
        ApplyMinFilterCubemap(cubemapFace, usesMipmaps);
        ApplyMagFilterCubemap(cubemapFace, usesMipmaps);
    }

    //Sets the currently-bound cubemap texture to use this setting's horizontal wrapping behavior.
    void ApplyHorzWrappingCubemap(CubeTextureTypes cubemapFace) const
    {
        glTexParameteri(TextureTypeToGLEnum(cubemapFace), GL_TEXTURE_WRAP_S, ToGLInt(HorzWrap));
    }
    //Sets the currently-bound cubemap texture to use this setting's vertical wrapping behavior.
    void ApplyVertWrappingCubemap(CubeTextureTypes cubemapFace) const
    {
        glTexParameteri(TextureTypeToGLEnum(cubemapFace), GL_TEXTURE_WRAP_T, ToGLInt(VertWrap));
    }
    //Sets the currently-bound cubemap texture to use this setting's horizontal/vertical wrapping behavior.
    void ApplyWrappingCubemap(CubeTextureTypes cubemapFace) const
    {
        ApplyHorzWrappingCubemap(cubemapFace);
        ApplyVertWrappingCubemap(cubemapFace);
    }

    //Applies all this instance's settings to the currently-bound cubemap texture.
    void ApplyAllSettingsCubemap(CubeTextureTypes cubemapFace, bool usesMipmaps) const
    {
        ApplyFilterCubemap(cubemapFace, usesMipmaps);
        ApplyWrappingCubemap(cubemapFace);
    }


private:

    static GLint ToGLInt(FilteringTypes tf, bool minFilter, bool genMips);
    static GLint ToGLInt(WrappingTypes twa);
};



//Settings for creating a 2D RGBA color texture.
struct ColorTextureSettings
{
public:

    //The different pixel sizes (in bytes).
    enum PixelSizes
    {
        CTS_8,
        CTS_16,
        CTS_32,

        CTS_8_GREYSCALE,
        CTS_16_GREYSCALE,
    };

    static GLenum ToInternalFormat(PixelSizes size);
    static std::string ToString(PixelSizes size);


    unsigned int Width, Height;
    PixelSizes PixelSize;
    TextureSettings BaseSettings;
    bool GenerateMipmaps;


    ColorTextureSettings(unsigned int width = 1, unsigned int height = 1, PixelSizes size = PixelSizes::CTS_32,
                         bool useMipmaps = true, TextureSettings settings = TextureSettings())
        : Width(width), Height(height), PixelSize(size), BaseSettings(settings), GenerateMipmaps(useMipmaps)
    {
        assert(settings.TextureType == TextureTypes::TT_2D);
    }

    //TODO: Move to DebugAssist. Same with DepthTextureSettings and the size arrays.
    std::string ToString(void) const
    {
        return std::string() + std::to_string(Width) + "x" + std::to_string(Height) + ", size: " + ToString(PixelSize);
    }
};


//Settings for creating a depth texture.
struct DepthTextureSettings
{
public:

    //The different pixel sizes (in bytes).
    enum PixelSizes
    {
        DTS_16,
        DTS_24,
        DTS_32,
    };

    static GLenum ToEnum(PixelSizes size);
    static std::string ToString(PixelSizes size);


    unsigned int Width, Height;
    PixelSizes PixelSize;
    TextureSettings BaseSettings;
    bool GenerateMipmaps;

    DepthTextureSettings(unsigned int width = 1, unsigned int height = 1, PixelSizes size = PixelSizes::DTS_24,
                         bool useMipmaps = true, TextureSettings settings = TextureSettings())
                         : Width(width), Height(height), PixelSize(size), BaseSettings(settings), GenerateMipmaps(useMipmaps)
    {

    }

    std::string ToString(void) const;
};