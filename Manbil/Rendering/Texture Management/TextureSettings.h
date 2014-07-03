#pragma once

#include "../../OpenGLIncludes.h"
#include "../../RenderDataHandler.h"


//PRIORITY: Pull out into .cpp.


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


    FilteringTypes MinFilter = FilteringTypes::MTF_NEAREST,
                   MagFilter = FilteringTypes::MTF_NEAREST;
    WrappingTypes HorzWrap = WrappingTypes::MTF_WRAP,
                  VertWrap = WrappingTypes::MTF_WRAP;


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


    static GLint ToGLInt(FilteringTypes tf, bool minFilter, bool genMips)
    {
        return (tf == FilteringTypes::MTF_LINEAR) ?
            ((minFilter && genMips) ?
        GL_LINEAR_MIPMAP_LINEAR :
                                GL_LINEAR) :
                                GL_NEAREST;
    }
    static GLint ToGLInt(WrappingTypes twa)
    {
        return (twa == WrappingTypes::MTF_CLAMP) ?
        GL_CLAMP_TO_EDGE :
                         GL_REPEAT;
    }
};




//TODO: Make RenderDataHandler and MTexture use these structs for creation.


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

    static GLenum ToInternalFormat(Sizes size)
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
    static GLenum ToFormat(Sizes size)
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
    static std::string ToString(Sizes size)
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

    static GLenum ToEnum(Sizes size)
    {
        switch (size)
        {
            case Sizes::DTS_16: return GL_DEPTH_COMPONENT16;
            case Sizes::DTS_24: return GL_DEPTH_COMPONENT24;
            case Sizes::DTS_32: return GL_DEPTH_COMPONENT32;
            default: return GL_INVALID_ENUM;
        }
    }
    static std::string ToString(Sizes size)
    {
        switch (size)
        {
            case Sizes::DTS_16: return "16";
            case Sizes::DTS_24: return "24";
            case Sizes::DTS_32: return "32";
            default: return "UNKNOWN_SIZE";
        }
    }


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







































//Handles manipulation of OpenGL textures.
class TextureSettings1
{
public:

	enum TextureFiltering
	{
		TF_NEAREST,
		TF_LINEAR,
	};
	enum TextureWrapping
	{
		TW_WRAP,
		TW_CLAMP,
	};

	TextureFiltering MagFilter, MinFilter;
	TextureWrapping HorWrap, VertWrap;

    bool GenerateMipmaps;


	TextureSettings1(TextureFiltering filter = TextureFiltering::TF_NEAREST, TextureWrapping wrap = TextureWrapping::TW_WRAP, bool genMipmaps = true)
        : MagFilter(filter), MinFilter(filter), HorWrap(wrap), VertWrap(wrap), GenerateMipmaps(genMipmaps)
	{

	}
	TextureSettings1(TextureFiltering mag, TextureFiltering min, TextureWrapping hor, TextureWrapping vert, bool genMipmaps = true)
		: MagFilter(mag), MinFilter(min), HorWrap(hor), VertWrap(vert), GenerateMipmaps(genMipmaps)
	{

	}


    //Uses the currently-bound texture.
    static void SetWrappingData(TextureWrapping horizontal, TextureWrapping vertical);
    //Uses the currently-bound texture.
    static void SetWrappingData(TextureWrapping wrap) { SetWrappingData(wrap, wrap); }
    //Uses the currently-bound texture.
    static void SetFilteringData(TextureFiltering minFilter, TextureFiltering magFilter, bool usesMipmaps);
    //Uses the currently-bound texture.
    static void SetFilteringData(TextureFiltering filter, bool usesMipmaps) { SetFilteringData(filter, filter, usesMipmaps); }
    //Uses the currently-bound texture.
    static void GenMipmaps(void);

    //Uses the currently-bound texture.
    void SetData(void) const;
    //Uses the currently-bound texture.
    void SetWrappingData(void) const { SetWrappingData(HorWrap, VertWrap); }
    //Uses the currently-bound texture.
    void SetFilteringData(void) const { SetFilteringData(MinFilter, MagFilter, GenerateMipmaps); }
    //Uses the currently-bound texture.
    void SetMipmaps(void) const { if (GenerateMipmaps) GenMipmaps(); }

    void SetData(RenderObjHandle glTex) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); SetData(); }
    void SetWrappingData(RenderObjHandle glTex) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); SetWrappingData(); }
    void SetFilteringData(RenderObjHandle glTex) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); SetFilteringData(); }
    void SetMipmaps(RenderObjHandle glTex) const { RenderDataHandler::BindTexture(TextureTypes::Tex_TwoD, glTex); SetMipmaps(); }


private:

	static GLint ToGLInt(TextureFiltering tf, bool minFilter, bool genMips) { return (tf == TextureFiltering::TF_LINEAR) ? ((minFilter && genMips) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR) : GL_NEAREST; }
	static GLint ToGLInt(TextureWrapping twa) { return (twa == TextureWrapping::TW_CLAMP) ? GL_CLAMP_TO_EDGE : GL_REPEAT; }
};