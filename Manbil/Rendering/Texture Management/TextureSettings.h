#pragma once

#include "../../OpenGLIncludes.h"
#include "../../RenderDataHandler.h"


//Handles texture settings.
struct TextureSettings
{
public:

    //The types of texture filtering.
    enum FilteringTypes
    {
        MTF_NEAREST,
        MTF_LINEAR,
    };

    //The types of texture wrapping.
    enum WrappingTypes
    {
        MTF_CLAMP,
        MTF_WRAP,
    };


    FilteringTypes MinFilter, MagFilter;
    WrappingTypes HorzWrap, VertWrap;


    TextureSettings(FilteringTypes min, FilteringTypes mag, WrappingTypes horz, WrappingTypes vert)
        : MinFilter(min), MagFilter(mag), HorzWrap(horz), VertWrap(vert)
    {

    }
    TextureSettings(FilteringTypes filter = FilteringTypes::MTF_NEAREST, WrappingTypes wrap = WrappingTypes::MTF_WRAP)
        : MinFilter(filter), MagFilter(filter), HorzWrap(wrap), VertWrap(wrap)
    {

    }


    //Sets the currently-bound texture to use this setting's min filter.
    void ApplyMinFilter(bool usesMipmaps) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGLInt(MinFilter, true, usesMipmaps));
    }
    //Sets the currently-bound texture to use this setting's mag filter.
    void ApplyMagFilter(bool usesMipmaps) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGLInt(MagFilter, false, usesMipmaps));
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGLInt(HorzWrap));
    }
    //Sets the currently-bound texture to use this setting's vertical wrapping behavior.
    void ApplyVertWrapping(void) const
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGLInt(VertWrap));
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


private:


    static GLint ToGLInt(FilteringTypes tf, bool minFilter, bool genMips);
    static GLint ToGLInt(WrappingTypes twa);
};



//Settings for creating an RGBA color texture.
struct ColorTextureSettings
{
public:

    //The different pixel sizes (in bytes).
    enum Sizes
    {
        CTS_8,
        CTS_16,
        CTS_32,

        CTS_8_GREYSCALE,
        CTS_16_GREYSCALE,
    };

    static GLenum ToInternalFormat(Sizes size);
    static GLenum ToFormat(Sizes size);
    static std::string ToString(Sizes size);


    unsigned int Width, Height;
    Sizes Size;
    TextureSettings BaseSettings;
    bool GenerateMipmaps;


    ColorTextureSettings(unsigned int width = 1, unsigned int height = 1, Sizes size = Sizes::CTS_32,
                         bool useMipmaps = true, TextureSettings settings = TextureSettings())
        : Width(width), Height(height), Size(size), BaseSettings(settings), GenerateMipmaps(useMipmaps)
    {

    }

    //TODO: Move to DebugAssist. Same with DepthTextureSettings and the size arrays.
    std::string ToString(void) const
    {
        return std::string() + std::to_string(Width) + "x" + std::to_string(Height) + ", size: " + ToString(Size);
    }
};


//Settings for creating a depth texture.
struct DepthTextureSettings
{
public:

    //The different pixel sizes (in bytes).
    enum Sizes
    {
        DTS_16,
        DTS_24,
        DTS_32,
    };

    static GLenum ToEnum(Sizes size);
    static std::string ToString(Sizes size);


    unsigned int Width, Height;
    Sizes Size;
    TextureSettings BaseSettings;
    bool GenerateMipmaps;

    DepthTextureSettings(unsigned int width = 1, unsigned int height = 1, Sizes size = Sizes::DTS_24,
                         bool useMipmaps = true, TextureSettings settings = TextureSettings())
        : Width(width), Height(height), Size(size), BaseSettings(settings), GenerateMipmaps(useMipmaps)
    {

    }

    std::string ToString(void) const;
};