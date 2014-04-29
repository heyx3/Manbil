#pragma once

#include "../../OpenGLIncludes.h"
#include "../../TextureSettings.h"


//Specifies settings for creating different kinds of textures.
//TODO: Make RenderDataHandler use these structs for creation.


//Settings for creating an RGBA color texture.
struct ColorTextureSettings
{
public:

    enum Sizes
    {
        CTS_8,
        CTS_16,
        CTS_32,
    };

    static GLenum ToEnum(Sizes size)
    {
        switch (size)
        {
            case Sizes::CTS_8: return GL_RGBA8;
            case Sizes::CTS_16: return GL_RGBA16;
            case Sizes::CTS_32: return GL_RGBA32F;
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

            default: return "UNKNOWN_SIZE";
        }
    }


    unsigned int Width, Height;
    Sizes Size;
    TextureSettings Settings;


    ColorTextureSettings(unsigned int width = 1, unsigned int height = 1,
                         Sizes size = Sizes::CTS_32,
                         TextureSettings settings = TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false))
        : Width(width), Height(height), Size(size), Settings(settings)
    {

    }

    std::string ToString(void) const
    {
        return std::string() + std::to_string(Width) + "x" + std::to_string(Height) +
               ", mipmapping: " + std::to_string(Settings.GenerateMipmaps) + ", size: " + ToString(Size);
    }
};


//Settings for creating a depth texture.
struct DepthTextureSettings
{
public:

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
    TextureSettings Settings;

    DepthTextureSettings(unsigned int width = 1, unsigned int height = 1,
                         Sizes size = Sizes::DTS_24,
                         TextureSettings settings = TextureSettings(TextureSettings::TF_NEAREST, TextureSettings::TW_CLAMP, false))
        : Width(width), Height(height), Size(size), Settings(settings)
    {

    }

    std::string ToString(void) const;
};