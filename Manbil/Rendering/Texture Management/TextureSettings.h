#pragma once

#include "../../OpenGLIncludes.h"
#include <assert.h>


//Information about how a texture is sampled.
struct TextureSampleSettings
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


    TextureSampleSettings(FilteringTypes min, FilteringTypes mag, WrappingTypes horz, WrappingTypes vert)
        : MinFilter(min), MagFilter(mag), HorzWrap(horz), VertWrap(vert) { }
    TextureSampleSettings(FilteringTypes filter = FilteringTypes::FT_NEAREST, WrappingTypes wrap = WrappingTypes::WT_WRAP)
        : MinFilter(filter), MagFilter(filter), HorzWrap(wrap), VertWrap(wrap) { }


    //Sets the currently-bound texture to use this setting's min filter.
    void ApplyMinFilter(TextureTypes type, bool usesMipmaps) const
    {
        glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_MIN_FILTER, ToGLInt(MinFilter, true, usesMipmaps));
    }
    //Sets the currently-bound texture to use this setting's mag filter.
    void ApplyMagFilter(TextureTypes type, bool usesMipmaps) const
    {
        glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_MAG_FILTER, ToGLInt(MagFilter, false, usesMipmaps));
    }
    //Sets the currently-bound texture to use this setting's min and mag filters.
    void ApplyFilter(TextureTypes type, bool usesMipmaps) const
    {
        ApplyMinFilter(type, usesMipmaps);
        ApplyMagFilter(type, usesMipmaps);
    }

    //Sets the currently-bound texture to use this setting's horizontal wrapping behavior.
    void ApplyHorzWrapping(TextureTypes type) const
    {
        glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_WRAP_S, ToGLInt(HorzWrap));
    }
    //Sets the currently-bound texture to use this setting's vertical wrapping behavior.
    void ApplyVertWrapping(TextureTypes type) const
    {
        glTexParameteri(TextureTypeToGLEnum(type), GL_TEXTURE_WRAP_T, ToGLInt(VertWrap));
    }
    //Sets the currently-bound texture to use this setting's horizontal/vertical wrapping behavior.
    void ApplyWrapping(TextureTypes type) const
    {
        ApplyHorzWrapping(type);
        ApplyVertWrapping(type);
    }

    //Applies all this instance's settings to the currently-bound texture.
    void ApplyAllSettings(TextureTypes type, bool usesMipmaps) const
    {
        ApplyFilter(type, usesMipmaps);
        ApplyWrapping(type);
    }


    //Sets the given face of the currently-bound cubemap texture to use this setting's min filter.
    void ApplyMinFilter(CubeTextureTypes cubemapFace, bool usesMipmaps) const
    {
        glTexParameteri(TextureTypeToGLEnum(cubemapFace), GL_TEXTURE_MIN_FILTER, ToGLInt(MinFilter, true, usesMipmaps));
    }
    //Sets the given face of the currently-bound cubemap texture to use this setting's mag filter.
    void ApplyMagFilter(CubeTextureTypes cubemapFace, bool usesMipmaps) const
    {
        glTexParameteri(TextureTypeToGLEnum(cubemapFace), GL_TEXTURE_MAG_FILTER, ToGLInt(MagFilter, false, usesMipmaps));
    }
    //Sets the given face of the currently-bound cubemap texture to use this setting's min and mag filters.
    void ApplyFilter(CubeTextureTypes cubemapFace, bool usesMipmaps) const
    {
        ApplyMinFilter(cubemapFace, usesMipmaps);
        ApplyMagFilter(cubemapFace, usesMipmaps);
    }

    //Sets the given face of the currently-bound cubemap texture to use this setting's horizontal wrapping behavior.
    void ApplyHorzWrapping(CubeTextureTypes cubemapFace) const
    {
        glTexParameteri(TextureTypeToGLEnum(cubemapFace), GL_TEXTURE_WRAP_S, ToGLInt(HorzWrap));
    }
    //Sets the given face of the currently-bound cubemap texture to use this setting's vertical wrapping behavior.
    void ApplyVertWrapping(CubeTextureTypes cubemapFace) const
    {
        glTexParameteri(TextureTypeToGLEnum(cubemapFace), GL_TEXTURE_WRAP_T, ToGLInt(VertWrap));
    }
    //Sets the given face of the currently-bound cubemap texture to use this setting's horizontal/vertical wrapping behavior.
    void ApplyWrapping(CubeTextureTypes cubemapFace) const
    {
        ApplyHorzWrapping(cubemapFace);
        ApplyVertWrapping(cubemapFace);
    }

    //Applies all this instance's settings to the currently-bound cubemap texture.
    void ApplyAllSettings(CubeTextureTypes cubemapFace, bool usesMipmaps) const
    {
        ApplyFilter(cubemapFace, usesMipmaps);
        ApplyWrapping(cubemapFace);
    }


private:

    static GLint ToGLInt(FilteringTypes tf, bool minFilter, bool genMips);
    static GLint ToGLInt(WrappingTypes twa);
};


//The different possible sizes of each component of a pixel in a texture.
//"Unsigned int" pixels are mapped from [0, max uint value] to [0, 1].
//"Float" pixels have the usual range of a floating-point number
//   (although keep in mind that 2-byte floats have a smaller range).
enum PixelSizes
{
    //RGBA, uint, 1 byte per component.
    PS_8U,
    //RGBA, uint, 2 bytes per component.
    PS_16U,

    //RGBA, float, 2 bytes per component.
    PS_16F,
    //RGBA, float, 4 bytes per component.
    PS_32F,


    //Red only, uint, 1 byte.
    PS_8U_GREYSCALE,
    //Red only, uint, 2 bytes.
    PS_16U_GREYSCALE,

    //Red only, float, 4 bytes.
    PS_32F_GREYSCALE,


    //Depth, uint, 2 bytes.
    PS_16U_DEPTH,
    //Depth, uint, 3 bytes.
    PS_24U_DEPTH,

    //Depth, uint, 4 bytes.
    PS_32F_DEPTH,
};
//Does the given pixel size use RGBA?
bool IsPixelSizeColored(PixelSizes pixelSize);
//Does the given pixel size only use the Red component (not RGBA)?
bool IsPixelSizeGreyscale(PixelSizes pixelSize);
//Does the given pixel size use a depth component (not RGBA)?
bool IsPixelSizeDepth(PixelSizes pixelSize);
//Does the given pixel size use a floating-point value (instead of unsigned int)?
bool IsPixelSizeFloat(PixelSizes pixelSize);
//Converts the given pixel size into its corresponding OpenGL enum value.
GLenum ToGLenum(PixelSizes pixelSize);
//Gets the size of a single pixel component in bytes for the given pixel size.
unsigned int GetComponentSize(PixelSizes pixelSize);