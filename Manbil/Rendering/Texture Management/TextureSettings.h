#pragma once

#include "../../OpenGLIncludes.h"
#include <assert.h>



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
bool IsPixelSizeColor(PixelSizes pixelSize);
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
//Outputs the given pixel size as a descriptive string.
std::string ToString(PixelSizes pixelSize);
//Gets the pixel size represented by the given descriptive string.
PixelSizes ToPixelSize(const std::string& pixelSizeToString);


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

GLint ToGLInt(FilteringTypes tf, bool minFilter, bool genMips);
GLint ToGLInt(WrappingTypes twa);


//Information about how a 2D texture is sampled.
struct TextureSampleSettings2D
{
public:

    FilteringTypes MinFilter, MagFilter;
    WrappingTypes HorzWrap, VertWrap;


    TextureSampleSettings2D(FilteringTypes min, FilteringTypes mag,
                            WrappingTypes horz, WrappingTypes vert)
        : MinFilter(min), MagFilter(mag), HorzWrap(horz), VertWrap(vert) { }
    TextureSampleSettings2D(FilteringTypes filter = FilteringTypes::FT_NEAREST,
                            WrappingTypes wrap = WrappingTypes::WT_WRAP)
        : MinFilter(filter), MagFilter(filter), HorzWrap(wrap), VertWrap(wrap) { }


    //Sets the currently-bound 2D texture to use this setting's min filter.
    void ApplyMinFilter(bool usesMipmaps) const;
    //Sets the currently-bound 2D texture to use this setting's mag filter.
    void ApplyMagFilter(bool usesMipmaps) const;
    //Sets the currently-bound 2D texture to use this setting's min and mag filters.
    void ApplyFilter(bool usesMipmaps) const;

    //Sets the currently-bound 2D texture to use this setting's horizontal wrapping behavior.
    void ApplyHorzWrapping(void) const;
    //Sets the currently-bound 2D texture to use this setting's vertical wrapping behavior.
    void ApplyVertWrapping(void) const;
    //Sets the currently-bound 2D texture to use this setting's horizontal/vertical wrapping behavior.
    void ApplyWrapping(void) const;

    //Applies all this instance's settings to the currently-bound 2D texture.
    void ApplyAllSettings(bool usesMipmaps) const;
};

//Information about how a 3D texture or cubemap texture is sampled.
struct TextureSampleSettings3D
{
public:

    FilteringTypes MinFilter, MagFilter;
    WrappingTypes XWrap, YWrap, ZWrap;


    TextureSampleSettings3D(FilteringTypes min, FilteringTypes mag,
                            WrappingTypes x, WrappingTypes y, WrappingTypes z)
        : MinFilter(min), MagFilter(mag), XWrap(x), YWrap(y), ZWrap(z) { }
    TextureSampleSettings3D(FilteringTypes filter = FilteringTypes::FT_NEAREST,
                            WrappingTypes wrap = WrappingTypes::WT_WRAP)
        : MinFilter(filter), MagFilter(filter), XWrap(wrap), YWrap(wrap), ZWrap(wrap) { }


    //Sets the currently-bound 3D/cubemap texture to use this setting's min filter.
    void ApplyMinFilter(TextureTypes type, bool usesMipmaps) const;
    //Sets the currently-bound 3D/cubemap texture to use this setting's mag filter.
    void ApplyMagFilter(TextureTypes type, bool usesMipmaps) const;
    //Sets the currently-bound 3D/cubemap texture to use this setting's min and mag filters.
    void ApplyFilter(TextureTypes type, bool usesMipmaps) const;

    //Sets the currently-bound 3D/cubemap texture to use this setting's X wrapping behavior.
    void ApplyXWrapping(TextureTypes type) const;
    //Sets the currently-bound 3D/cubemap texture to use this setting's Y wrapping behavior.
    void ApplyYWrapping(TextureTypes type) const;
    //Sets the currently-bound 3D/cubemap texture to use this setting's Y wrapping behavior.
    void ApplyZWrapping(TextureTypes type) const;
    //Sets the currently-bound 3D/cubemap texture to use this setting's XYZ wrapping behavior.
    void ApplyWrapping(TextureTypes type) const;

    //Applies all this instance's settings to the currently-bound 3D/cubemap texture.
    void ApplyAllSettings(TextureTypes type, bool usesMipmaps) const;
};